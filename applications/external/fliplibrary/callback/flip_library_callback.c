#include <callback/flip_library_callback.h>
uint32_t random_facts_index = 0;
bool sent_random_fact_request = false;
bool random_fact_request_success = false;
bool random_fact_request_success_all = false;
char* random_fact = NULL;

// Parse JSON to find the "text" key
char* flip_library_parse_random_fact() {
    return get_json_value("text", fhttp.last_response, 128);
}

char* flip_library_parse_cat_fact() {
    return get_json_value("fact", fhttp.last_response, 128);
}

char* flip_library_parse_dog_fact() {
    return get_json_array_value("facts", 0, fhttp.last_response, 256);
}

char* flip_library_parse_quote() {
    // remove [ and ] from the start and end of the string
    char* response = fhttp.last_response;
    if(response[0] == '[') {
        response++;
    }
    if(response[strlen(response) - 1] == ']') {
        response[strlen(response) - 1] = '\0';
    }
    // remove white space from both sides
    while(response[0] == ' ') {
        response++;
    }
    while(response[strlen(response) - 1] == ' ') {
        response[strlen(response) - 1] = '\0';
    }
    return get_json_value("q", response, 128);
}

char* flip_library_parse_dictionary() {
    return get_json_value("definition", fhttp.last_response, 16);
}

void flip_library_request_error(Canvas* canvas) {
    if(fhttp.last_response == NULL) {
        if(fhttp.last_response != NULL) {
            if(strstr(fhttp.last_response, "[ERROR] Not connected to Wifi. Failed to reconnect.") !=
               NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
                canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else if(strstr(fhttp.last_response, "[ERROR] Failed to connect to Wifi.") != NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
                canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else {
                canvas_clear(canvas);
                FURI_LOG_E(TAG, "Received an error: %s", fhttp.last_response);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Unusual error...");
                canvas_draw_str(canvas, 0, 60, "Press BACK and retry.");
            }
        } else {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "[ERROR] Unknown error.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        }
    } else {
        canvas_clear(canvas);
        canvas_draw_str(canvas, 0, 10, "Failed to receive data.");
        canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
    }
}

void flip_library_draw_fact(char* message, Widget** widget) {
    if(app_instance == NULL) {
        FURI_LOG_E(TAG, "App instance is NULL");
        return;
    }
    widget_reset(*widget);

    uint32_t fact_length = strlen(message); // Length of the message
    uint32_t i = 0; // Index tracker
    uint32_t formatted_index = 0; // Tracker for where we are in the formatted message
    char* formatted_message; // Buffer to hold the final formatted message
    if(!easy_flipper_set_buffer(&formatted_message, fact_length * 2 + 1)) {
        return;
    }

    while(i < fact_length) {
        uint32_t max_line_length = 29; // Maximum characters per line
        uint32_t remaining_length = fact_length - i; // Remaining characters
        uint32_t line_length = (remaining_length < max_line_length) ? remaining_length :
                                                                      max_line_length;

        // Temporary buffer to hold the current line
        char fact_line[30];
        strncpy(fact_line, message + i, line_length);
        fact_line[line_length] = '\0';

        // Check if the line ends in the middle of a word and adjust accordingly
        if(line_length == 29 && message[i + line_length] != '\0' &&
           message[i + line_length] != ' ') {
            // Find the last space within the 30-character segment
            char* last_space = strrchr(fact_line, ' ');
            if(last_space != NULL) {
                // Adjust the line length to avoid cutting the word
                line_length = last_space - fact_line;
                fact_line[line_length] = '\0'; // Null-terminate at the space
            }
        }

        // Manually copy the fixed line into the formatted_message buffer
        for(uint32_t j = 0; j < line_length; j++) {
            formatted_message[formatted_index++] = fact_line[j];
        }

        // Add a newline character for line spacing
        formatted_message[formatted_index++] = '\n';

        // Move i forward to the start of the next word
        i += line_length;

        // Skip spaces at the beginning of the next line
        while(message[i] == ' ') {
            i++;
        }
    }

    // Add the formatted message to the widget
    widget_add_text_scroll_element(*widget, 0, 0, 128, 64, formatted_message);
}

// Callback for drawing the main screen
void view_draw_callback_random_facts(Canvas* canvas, void* model) {
    if(!canvas || !app_instance) {
        return;
    }
    UNUSED(model);

    canvas_set_font(canvas, FontSecondary);

    if(fhttp.state == INACTIVE) {
        canvas_draw_str(canvas, 0, 7, "Wifi Dev Board disconnected.");
        canvas_draw_str(canvas, 0, 17, "Please connect to the board.");
        canvas_draw_str(canvas, 0, 32, "If your board is connected,");
        canvas_draw_str(canvas, 0, 42, "make sure you have flashed");
        canvas_draw_str(canvas, 0, 52, "your WiFi Devboard with the");
        canvas_draw_str(canvas, 0, 62, "latest FlipperHTTP flash.");
        return;
    }
    // Cats
    if(random_facts_index == FlipLibrarySubmenuIndexRandomFactsCats) {
        canvas_draw_str(canvas, 0, 7, "Random Cat Fact");
        canvas_draw_str(canvas, 0, 15, "Loading...");

        if(!sent_random_fact_request) {
            sent_random_fact_request = true;
            random_fact_request_success = flipper_http_get_request_with_headers(
                "https://catfact.ninja/fact", "{\"Content-Type\":\"application/json\"}");
            if(!random_fact_request_success) {
                FURI_LOG_E(TAG, "Failed to send request");
                flip_library_request_error(canvas);
                return;
            }
            fhttp.state = RECEIVING;
        } else {
            if(fhttp.state == RECEIVING) {
                canvas_draw_str(canvas, 0, 22, "Receiving...");
                return;
            }
            // check status
            else if(fhttp.state == ISSUE || !random_fact_request_success) {
                flip_library_request_error(canvas);
            } else if(
                fhttp.state == IDLE && fhttp.last_response != NULL &&
                !random_fact_request_success_all) {
                canvas_draw_str(canvas, 0, 22, "Processing...");
                // success
                // check status
                // unnecessary check
                if(fhttp.state == ISSUE || fhttp.last_response == NULL) {
                    flip_library_request_error(canvas);
                    FURI_LOG_E(TAG, "HTTP request failed or received data is NULL");
                    return;
                } else if(!random_fact_request_success_all) {
                    random_fact = flip_library_parse_cat_fact();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }

                    // Mark success
                    random_fact_request_success_all = true;

                    // draw random facts
                    flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                    // go to random facts widget
                    view_dispatcher_switch_to_view(
                        app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
                }
            }
            // likely redundant but just in case
            else if(fhttp.state == IDLE && random_fact_request_success_all && random_fact != NULL) {
                flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
            } else // handle weird scenarios
            {
                // if received data isnt NULL
                if(fhttp.last_response != NULL) {
                    // parse json to find the text key
                    random_fact = flip_library_parse_cat_fact();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }
                }
            }
        }
    }
    // Dogs
    else if(random_facts_index == FlipLibrarySubmenuIndexRandomFactsDogs) {
        canvas_draw_str(canvas, 0, 7, "Random Dog Fact");
        canvas_draw_str(canvas, 0, 15, "Loading...");

        if(!sent_random_fact_request) {
            sent_random_fact_request = true;
            random_fact_request_success = flipper_http_get_request_with_headers(
                "https://dog-api.kinduff.com/api/facts",
                "{\"Content-Type\":\"application/json\"}");
            if(!random_fact_request_success) {
                FURI_LOG_E(TAG, "Failed to send request");
                flip_library_request_error(canvas);
                fhttp.state = ISSUE;
                return;
            }
            fhttp.state = RECEIVING;
        } else {
            if(fhttp.state == RECEIVING) {
                canvas_draw_str(canvas, 0, 22, "Receiving...");
                return;
            }
            // check status
            else if(fhttp.state == ISSUE || !random_fact_request_success) {
                flip_library_request_error(canvas);
            } else if(
                fhttp.state == IDLE && fhttp.last_response != NULL &&
                !random_fact_request_success_all) {
                canvas_draw_str(canvas, 0, 22, "Processing...");
                // success
                // check status
                // unnecessary check
                if(fhttp.state == ISSUE || fhttp.last_response == NULL) {
                    flip_library_request_error(canvas);
                    FURI_LOG_E(TAG, "HTTP request failed or received data is NULL");
                    return;
                } else if(!random_fact_request_success_all) {
                    random_fact = flip_library_parse_dog_fact();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }

                    // Mark success
                    random_fact_request_success_all = true;

                    // draw random facts
                    flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                    // go to random facts widget
                    view_dispatcher_switch_to_view(
                        app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
                }
            }
            // likely redundant but just in case
            else if(fhttp.state == IDLE && random_fact_request_success_all && random_fact != NULL) {
                flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
            } else // handle weird scenarios
            {
                // if received data isnt NULL
                if(fhttp.last_response != NULL) {
                    // parse json to find the text key
                    random_fact = flip_library_parse_cat_fact();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }
                }
            }
        }
    }
    // Quotes
    else if(random_facts_index == FlipLibrarySubmenuIndexRandomFactsQuotes) {
        canvas_draw_str(canvas, 0, 7, "Random Quote");
        canvas_draw_str(canvas, 0, 15, "Loading...");

        if(!sent_random_fact_request) {
            sent_random_fact_request = true;
            random_fact_request_success =
                flipper_http_get_request("https://zenquotes.io/api/random");
            if(!random_fact_request_success) {
                FURI_LOG_E(TAG, "Failed to send request");
                flip_library_request_error(canvas);
                return;
            }
            fhttp.state = RECEIVING;
        } else {
            if(fhttp.state == RECEIVING) {
                canvas_draw_str(canvas, 0, 22, "Receiving...");
                return;
            }
            // check status
            else if(fhttp.state == ISSUE || !random_fact_request_success) {
                flip_library_request_error(canvas);
            } else if(
                fhttp.state == IDLE && fhttp.last_response != NULL &&
                !random_fact_request_success_all) {
                canvas_draw_str(canvas, 0, 22, "Processing...");
                // success
                // check status
                // unnecessary check
                if(fhttp.state == ISSUE || fhttp.last_response == NULL) {
                    flip_library_request_error(canvas);
                    FURI_LOG_E(TAG, "HTTP request failed or received data is NULL");
                    return;
                } else if(!random_fact_request_success_all) {
                    random_fact = flip_library_parse_quote();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }

                    // Mark success
                    random_fact_request_success_all = true;

                    // draw random facts
                    flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                    // go to random facts widget
                    view_dispatcher_switch_to_view(
                        app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
                }
            }
            // likely redundant but just in case
            else if(fhttp.state == IDLE && random_fact_request_success_all && random_fact != NULL) {
                flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
            } else // handle weird scenarios
            {
                // if received data isnt NULL
                if(fhttp.last_response != NULL) {
                    // parse json to find the text key
                    random_fact = flip_library_parse_quote();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }
                }
            }
        }
    }
    // All Random Facts
    else if(random_facts_index == FlipLibrarySubmenuIndexRandomFactsAll) {
        canvas_draw_str(canvas, 0, 10, "Random Fact");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 20, "Loading...");

        if(!sent_random_fact_request) {
            sent_random_fact_request = true;

            random_fact_request_success =
                flipper_http_get_request("https://uselessfacts.jsph.pl/api/v2/facts/random");
            if(!random_fact_request_success) {
                FURI_LOG_E(TAG, "Failed to send request");
                fhttp.state = ISSUE;
                return;
            }
            fhttp.state = RECEIVING;
        } else {
            // check status
            if(fhttp.state == RECEIVING) {
                canvas_draw_str(canvas, 0, 30, "Receiving...");
                return;
            }
            // check status
            else if(fhttp.state == ISSUE || !random_fact_request_success) {
                flip_library_request_error(canvas);
                return;
            } else if(
                fhttp.state == IDLE && fhttp.last_response != NULL &&
                !random_fact_request_success_all) {
                canvas_draw_str(canvas, 0, 30, "Processing...");
                // success
                // check status
                if(fhttp.state == ISSUE || fhttp.last_response == NULL) {
                    flip_library_request_error(canvas);
                    FURI_LOG_E(TAG, "HTTP request failed or received data is NULL");
                    return;
                }

                // parse json to find the text key
                random_fact = flip_library_parse_random_fact();

                if(random_fact == NULL) {
                    flip_library_request_error(canvas);
                    fhttp.state = ISSUE;
                    return;
                }

                // Mark success
                random_fact_request_success_all = true;

                // draw random facts
                flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
            }
            // likely redundant but just in case
            else if(fhttp.state == IDLE && random_fact_request_success_all && random_fact != NULL) {
                // draw random facts
                flip_library_draw_fact(random_fact, &app_instance->widget_random_fact);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewRandomFactWidget);
            } else // handle weird scenarios
            {
                // if received data isnt NULL
                if(fhttp.last_response != NULL) {
                    // parse json to find the text key
                    random_fact = flip_library_parse_random_fact();

                    if(random_fact == NULL) {
                        flip_library_request_error(canvas);
                        fhttp.state = ISSUE;
                        return;
                    }
                }
            }
        }
    } else {
        canvas_draw_str(canvas, 0, 7, "Random Fact");
    }
}

void view_draw_callback_dictionary_run(Canvas* canvas, void* model) {
    if(!canvas || !app_instance || app_instance->uart_text_input_buffer_dictionary == NULL) {
        return;
    }

    UNUSED(model);

    canvas_set_font(canvas, FontSecondary);

    if(fhttp.state == INACTIVE) {
        canvas_draw_str(canvas, 0, 7, "Wifi Dev Board disconnected.");
        canvas_draw_str(canvas, 0, 17, "Please connect to the board.");
        canvas_draw_str(canvas, 0, 32, "If your board is connected,");
        canvas_draw_str(canvas, 0, 42, "make sure you have flashed");
        canvas_draw_str(canvas, 0, 52, "your WiFi Devboard with the");
        canvas_draw_str(canvas, 0, 62, "latest FlipperHTTP flash.");
        return;
    }

    canvas_draw_str(canvas, 0, 10, "Defining, please wait...");

    if(!sent_random_fact_request) {
        sent_random_fact_request = true;

        char payload[128];
        snprintf(
            payload,
            sizeof(payload),
            "{\"word\":\"%s\"}",
            app_instance->uart_text_input_buffer_dictionary);

        random_fact_request_success = flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/define/",
            "{\"Content-Type\":\"application/json\"}",
            payload);
        if(!random_fact_request_success) {
            FURI_LOG_E(TAG, "Failed to send request");
            flip_library_request_error(canvas);
            fhttp.state = ISSUE;
            return;
        }
        fhttp.state = RECEIVING;
    } else {
        // check status
        if(fhttp.state == RECEIVING) {
            canvas_draw_str(canvas, 0, 20, "Receiving...");
            return;
        }
        // check status
        else if(fhttp.state == ISSUE || !random_fact_request_success) {
            flip_library_request_error(canvas);
            return;
        } else if(
            fhttp.state == IDLE && fhttp.last_response != NULL &&
            !random_fact_request_success_all) {
            canvas_draw_str(canvas, 0, 20, "Processing...");
            // success
            // check status
            if(fhttp.state == ISSUE || fhttp.last_response == NULL) {
                flip_library_request_error(canvas);
                FURI_LOG_E(TAG, "HTTP request failed or received data is NULL");
                return;
            }

            // parse json to find the text key
            char* definition = flip_library_parse_dictionary();

            if(definition == NULL) {
                flip_library_request_error(canvas);
                fhttp.state = ISSUE;
                return;
            }

            // Mark success
            random_fact_request_success_all = true;

            // draw random facts
            flip_library_draw_fact(definition, &app_instance->widget_dictionary);

            // go to random facts widget
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipLibraryViewDictionaryWidget);
        }
        // likely redundant but just in case
        else if(fhttp.state == IDLE && random_fact_request_success_all && random_fact != NULL) {
            // draw random facts
            flip_library_draw_fact(random_fact, &app_instance->widget_dictionary);

            // go to random facts widget
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipLibraryViewDictionaryWidget);
        } else // handle weird scenarios
        {
            // if received data isnt NULL
            if(fhttp.last_response != NULL) {
                // parse json to find the text key
                char* definition = flip_library_parse_dictionary();

                if(definition == NULL) {
                    flip_library_request_error(canvas);
                    fhttp.state = ISSUE;
                    return;
                }

                // draw random facts
                flip_library_draw_fact(definition, &app_instance->widget_dictionary);

                // go to random facts widget
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipLibraryViewDictionaryWidget);

                free(definition);

                return;
            }
        }
    }
}

// Input callback for the view (async input handling)
bool view_input_callback_random_facts(InputEvent* event, void* context) {
    if(!event || !context) {
        return false;
    }
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(event->type == InputTypePress && event->key == InputKeyBack) {
        // Exit the app when the back button is pressed
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    }
    return false;
}

void callback_submenu_choices(void* context, uint32_t index) {
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }
    switch(index) {
    case FlipLibrarySubmenuIndexRandomFacts:
        random_facts_index = 0;
        sent_random_fact_request = false;
        random_fact = NULL;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewRandomFacts);
        break;
    case FlipLibrarySubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewAbout);
        break;
    case FlipLibrarySubmenuIndexSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewSettings);
        break;
    case FlipLibrarySubmenuIndexDictionary:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewDictionaryTextInput);
        break;
    case FlipLibrarySubmenuIndexRandomFactsCats:
        random_facts_index = FlipLibrarySubmenuIndexRandomFactsCats;
        sent_random_fact_request = false;
        random_fact = NULL;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewRandomFactsRun);
        break;
    case FlipLibrarySubmenuIndexRandomFactsDogs:
        random_facts_index = FlipLibrarySubmenuIndexRandomFactsDogs;
        sent_random_fact_request = false;
        random_fact = NULL;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewRandomFactsRun);
        break;
    case FlipLibrarySubmenuIndexRandomFactsQuotes:
        random_facts_index = FlipLibrarySubmenuIndexRandomFactsQuotes;
        sent_random_fact_request = false;
        random_fact = NULL;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewRandomFactsRun);
        break;
    case FlipLibrarySubmenuIndexRandomFactsAll:
        random_facts_index = FlipLibrarySubmenuIndexRandomFactsAll;
        sent_random_fact_request = false;
        random_fact = NULL;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewRandomFactsRun);
        break;
    default:
        break;
    }
}

void text_updated_ssid(void* context) {
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_ssid,
        app->uart_text_input_temp_buffer_ssid,
        app->uart_text_input_buffer_size_ssid);

    // Ensure null-termination
    app->uart_text_input_buffer_ssid[app->uart_text_input_buffer_size_ssid - 1] = '\0';

    // update the variable item text
    if(app->variable_item_ssid) {
        variable_item_set_current_value_text(
            app->variable_item_ssid, app->uart_text_input_buffer_ssid);
    }

    // save settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password);

    // save wifi settings to devboard
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_password) > 0) {
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewSettings);
}

void text_updated_password(void* context) {
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_password,
        app->uart_text_input_temp_buffer_password,
        app->uart_text_input_buffer_size_password);

    // Ensure null-termination
    app->uart_text_input_buffer_password[app->uart_text_input_buffer_size_password - 1] = '\0';

    // update the variable item text
    if(app->variable_item_password) {
        variable_item_set_current_value_text(
            app->variable_item_password, app->uart_text_input_buffer_password);
    }

    // save settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password);

    // save wifi settings to devboard
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_password) > 0) {
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewSettings);
}

void text_updated_dictionary(void* context) {
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_dictionary,
        app->uart_text_input_temp_buffer_dictionary,
        app->uart_text_input_buffer_size_dictionary);

    // Ensure null-termination
    app->uart_text_input_buffer_dictionary[app->uart_text_input_buffer_size_dictionary - 1] = '\0';

    // switch to the dictionary view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewDictionaryRun);
}

uint32_t callback_to_submenu(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    random_facts_index = 0;
    sent_random_fact_request = false;
    random_fact_request_success = false;
    random_fact_request_success_all = false;
    random_fact = NULL;
    return FlipLibraryViewSubmenuMain;
}

uint32_t callback_to_wifi_settings(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return FlipLibraryViewSettings;
}

uint32_t callback_to_random_facts(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return FlipLibraryViewRandomFacts;
}

void settings_item_selected(void* context, uint32_t index) {
    FlipLibraryApp* app = (FlipLibraryApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewTextInputSSID);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewTextInputPassword);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Navigation callback for exiting the application
 * @param context The context - unused
 * @return next view id (VIEW_NONE to exit the app)
 */
uint32_t callback_exit_app(void* context) {
    // Exit the application
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return VIEW_NONE; // Return VIEW_NONE to exit the app
}
