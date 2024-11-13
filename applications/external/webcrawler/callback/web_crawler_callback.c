#include <callback/web_crawler_callback.h>
bool sent_http_request = false;
bool get_success = false;
bool already_success = false;

void web_crawler_draw_error(Canvas* canvas) {
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    canvas_clear(canvas);
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

    if(fhttp.last_response) {
        if(strstr(fhttp.last_response, "[ERROR] Not connected to Wifi. Failed to reconnect.") !=
           NULL) {
            canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }
        if(strstr(fhttp.last_response, "[ERROR] Failed to connect to Wifi.") != NULL) {
            canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }
        if(strstr(
               fhttp.last_response, "[ERROR] GET request failed with error: connection refused") !=
           NULL) {
            canvas_draw_str(canvas, 0, 10, "[ERROR] Connection refused.");
            canvas_draw_str(canvas, 0, 50, "Choose another URL.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }

        canvas_draw_str(canvas, 0, 10, "[ERROR] Failed to sync data.");
        canvas_draw_str(canvas, 0, 30, "If this is your third attempt,");
        canvas_draw_str(canvas, 0, 40, "it's likely your URL is not");
        canvas_draw_str(canvas, 0, 50, "compabilbe or correct.");
        canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        return;
    }

    canvas_draw_str(canvas, 0, 10, "HTTP request failed.");
    canvas_draw_str(canvas, 0, 20, "Press BACK to return.");
}

void web_crawler_http_method_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, http_method_names[index]);

    // save the http method
    if(app_instance) {
        strncpy(
            app_instance->http_method,
            http_method_names[index],
            strlen(http_method_names[index]) + 1);

        // save the settings
        save_settings(
            app_instance->path,
            app_instance->ssid,
            app_instance->password,
            app_instance->file_rename,
            app_instance->file_type,
            app_instance->http_method,
            app_instance->headers,
            app_instance->payload);
    }
}

void web_crawler_view_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    if(!app_instance) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    if(fhttp.state == INACTIVE || fhttp.state == ISSUE) {
        web_crawler_draw_error(canvas);
        return;
    }

    if(app_instance->path) {
        if(!sent_http_request) {
            if(app_instance->file_type && app_instance->file_rename) {
                snprintf(
                    fhttp.file_path,
                    sizeof(fhttp.file_path),
                    STORAGE_EXT_PATH_PREFIX "/apps_data/web_crawler/%s%s",
                    app_instance->file_rename,
                    app_instance->file_type);
            } else {
                snprintf(
                    fhttp.file_path,
                    sizeof(fhttp.file_path),
                    STORAGE_EXT_PATH_PREFIX "/apps_data/web_crawler/received_data.txt");
            }

            if(strstr(app_instance->http_method, "GET") != NULL) {
                canvas_draw_str(canvas, 0, 10, "Sending GET request...");

                fhttp.save_received_data = true;
                fhttp.is_bytes_request = false;

                // Perform GET request and handle the response
                if(app_instance->headers == NULL || app_instance->headers[0] == '\0' ||
                   strstr(app_instance->headers, " ") == NULL) {
                    get_success = flipper_http_get_request(app_instance->path);
                } else {
                    get_success = flipper_http_get_request_with_headers(
                        app_instance->path, app_instance->headers);
                }
            } else if(strstr(app_instance->http_method, "POST") != NULL) {
                canvas_draw_str(canvas, 0, 10, "Sending POST request...");

                fhttp.save_received_data = true;
                fhttp.is_bytes_request = false;

                // Perform POST request and handle the response
                get_success = flipper_http_post_request_with_headers(
                    app_instance->path, app_instance->headers, app_instance->payload);
            } else if(strstr(app_instance->http_method, "PUT") != NULL) {
                canvas_draw_str(canvas, 0, 10, "Sending PUT request...");

                fhttp.save_received_data = true;
                fhttp.is_bytes_request = false;

                // Perform PUT request and handle the response
                get_success = flipper_http_put_request_with_headers(
                    app_instance->path, app_instance->headers, app_instance->payload);
            } else if(strstr(app_instance->http_method, "DELETE") != NULL) {
                canvas_draw_str(canvas, 0, 10, "Sending DELETE request...");

                fhttp.save_received_data = true;
                fhttp.is_bytes_request = false;

                // Perform DELETE request and handle the response
                get_success = flipper_http_delete_request_with_headers(
                    app_instance->path, app_instance->headers, app_instance->payload);
            } else {
                // download file
                canvas_draw_str(canvas, 0, 10, "Downloading file...");

                fhttp.save_received_data = false;
                fhttp.is_bytes_request = true;

                // Perform GET request and handle the response
                get_success =
                    flipper_http_get_request_bytes(app_instance->path, app_instance->headers);
            }

            canvas_draw_str(canvas, 0, 20, "Sent!");

            if(get_success) {
                canvas_draw_str(canvas, 0, 30, "Receiving data...");
                fhttp.state = RECEIVING;
            } else {
                canvas_draw_str(canvas, 0, 30, "Failed.");
                fhttp.state = ISSUE;
            }

            sent_http_request = true;
        } else {
            // print state
            if(get_success && fhttp.state == RECEIVING) {
                canvas_draw_str(canvas, 0, 10, "Receiving and parsing data...");
            } else if(get_success && fhttp.state == IDLE) {
                canvas_draw_str(canvas, 0, 10, "Data saved to file.");
                canvas_draw_str(canvas, 0, 20, "Press BACK to return.");
            } else {
                web_crawler_draw_error(canvas);
                get_success = false;
            }
        }
    } else {
        canvas_draw_str(canvas, 0, 10, "URL not set.");
    }
}

/**
 * @brief      Navigation callback to handle exiting from other views to the submenu.
 * @param      context   The context - WebCrawlerApp object.
 * @return     WebCrawlerViewSubmenu
 */
uint32_t web_crawler_back_to_configure_callback(void* context) {
    UNUSED(context);
    // free file read widget if it exists
    if(app_instance->widget_file_read) {
        widget_reset(app_instance->widget_file_read);
    }
    return WebCrawlerViewSubmenuConfig; // Return to the configure screen
}

/**
 * @brief      Navigation callback to handle returning to the Wifi Settings screen.
 * @param      context   The context - WebCrawlerApp object.
 * @return     WebCrawlerViewSubmenu
 */
uint32_t web_crawler_back_to_main_callback(void* context) {
    UNUSED(context);
    // reset GET request flags
    sent_http_request = false;
    get_success = false;
    already_success = false;
    // free file read widget if it exists
    if(app_instance->widget_file_read) {
        widget_reset(app_instance->widget_file_read);
    }
    return WebCrawlerViewSubmenuMain; // Return to the main submenu
}

uint32_t web_crawler_back_to_file_callback(void* context) {
    UNUSED(context);
    return WebCrawlerViewVariableItemListFile; // Return to the file submenu
}

uint32_t web_crawler_back_to_wifi_callback(void* context) {
    UNUSED(context);
    return WebCrawlerViewVariableItemListWifi; // Return to the wifi submenu
}

uint32_t web_crawler_back_to_request_callback(void* context) {
    UNUSED(context);
    return WebCrawlerViewVariableItemListRequest; // Return to the request submenu
}

/**
 * @brief      Navigation callback to handle exiting the app from the main submenu.
 * @param      context   The context - unused
 * @return     VIEW_NONE to exit the app
 */
uint32_t web_crawler_exit_app_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

/**
 * @brief      Handle submenu item selection.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The WebCrawlerSubmenuIndex item that was clicked.
 */
void web_crawler_submenu_callback(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;

    if(app->view_dispatcher) {
        switch(index) {
        case WebCrawlerSubmenuIndexRun:
            sent_http_request = false; // Reset the flag
            view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewRun);
            break;
        case WebCrawlerSubmenuIndexAbout:
            view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewAbout);
            break;
        case WebCrawlerSubmenuIndexConfig:
            view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewSubmenuConfig);
            break;
        case WebCrawlerSubmenuIndexWifi:
            view_dispatcher_switch_to_view(
                app->view_dispatcher, WebCrawlerViewVariableItemListWifi);
            break;
        case WebCrawlerSubmenuIndexRequest:
            view_dispatcher_switch_to_view(
                app->view_dispatcher, WebCrawlerViewVariableItemListRequest);
            break;
        case WebCrawlerSubmenuIndexFile:
            view_dispatcher_switch_to_view(
                app->view_dispatcher, WebCrawlerViewVariableItemListFile);
            break;
        default:
            FURI_LOG_E(TAG, "Unknown submenu index");
            break;
        }
    }
}

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_wifi_enter_callback(void* context, uint32_t index) {
    switch(index) {
    case 0: // SSID
        web_crawler_setting_item_ssid_clicked(context, index);
        break;
    case 1: // Password
        web_crawler_setting_item_password_clicked(context, index);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_file_enter_callback(void* context, uint32_t index) {
    switch(index) {
    case 0: // File Read
        web_crawler_setting_item_file_read_clicked(context, index);
        break;
    case 1: // FIle Type
        web_crawler_setting_item_file_type_clicked(context, index);
        break;
    case 2: // File Rename
        web_crawler_setting_item_file_rename_clicked(context, index);
        break;
    case 3: // File Delete
        web_crawler_setting_item_file_delete_clicked(context, index);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_request_enter_callback(void* context, uint32_t index) {
    switch(index) {
    case 0: // URL
        web_crawler_setting_item_path_clicked(context, index);
        break;
    case 1:
        // HTTP Method
        break;
    case 2:
        // Headers
        web_crawler_setting_item_headers_clicked(context, index);
        break;
    case 3:
        // Payload
        web_crawler_setting_item_payload_clicked(context, index);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief      Callback for when the user finishes entering the URL.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_path_updated(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->path || !app->temp_buffer_path || !app->temp_buffer_size_path || !app->path_item) {
        FURI_LOG_E(TAG, "Invalid path buffer");
        return;
    }
    // Store the entered URL from temp_buffer_path to path
    strncpy(app->path, app->temp_buffer_path, app->temp_buffer_size_path - 1);

    if(app->path_item) {
        variable_item_set_current_value_text(app->path_item, app->path);

        // Save the URL to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListRequest);
}

/**
 * @brief      Callback for when the user finishes entering the headers
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_headers_updated(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_headers || !app->temp_buffer_size_headers || !app->headers_item) {
        FURI_LOG_E(TAG, "Invalid headers buffer");
        return;
    }
    // Store the entered headers from temp_buffer_headers to headers
    strncpy(app->headers, app->temp_buffer_headers, app->temp_buffer_size_headers - 1);

    if(app->headers_item) {
        variable_item_set_current_value_text(app->headers_item, app->headers);

        // Save the headers to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListRequest);
}

/**
 * @brief      Callback for when the user finishes entering the payload.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_payload_updated(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_payload || !app->temp_buffer_size_payload || !app->payload_item) {
        FURI_LOG_E(TAG, "Invalid payload buffer");
        return;
    }
    // Store the entered payload from temp_buffer_payload to payload
    strncpy(app->payload, app->temp_buffer_payload, app->temp_buffer_size_payload - 1);

    if(app->payload_item) {
        variable_item_set_current_value_text(app->payload_item, app->payload);

        // Save the payload to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListRequest);
}

/**
 * @brief      Callback for when the user finishes entering the SSID.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_ssid_updated(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_ssid || !app->temp_buffer_size_ssid || !app->ssid || !app->ssid_item) {
        FURI_LOG_E(TAG, "Invalid SSID buffer");
        return;
    }
    // Store the entered SSID from temp_buffer_ssid to ssid
    strncpy(app->ssid, app->temp_buffer_ssid, app->temp_buffer_size_ssid - 1);

    if(app->ssid_item) {
        variable_item_set_current_value_text(app->ssid_item, app->ssid);

        // Save the SSID to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);

        // send to UART
        if(!flipper_http_save_wifi(app->ssid, app->password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
            FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        }
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListWifi);
}

/**
 * @brief      Callback for when the user finishes entering the Password.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_password_update(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_password || !app->temp_buffer_size_password || !app->password ||
       !app->password_item) {
        FURI_LOG_E(TAG, "Invalid password buffer");
        return;
    }
    // Store the entered Password from temp_buffer_password to password
    strncpy(app->password, app->temp_buffer_password, app->temp_buffer_size_password - 1);

    if(app->password_item) {
        variable_item_set_current_value_text(app->password_item, app->password);

        // Save the Password to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);

        // send to UART
        if(!flipper_http_save_wifi(app->ssid, app->password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
            FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        }
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListWifi);
}

/**
 * @brief      Callback for when the user finishes entering the File Type.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_file_type_update(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_file_type || !app->temp_buffer_size_file_type || !app->file_type ||
       !app->file_type_item) {
        FURI_LOG_E(TAG, "Invalid file type buffer");
        return;
    }
    // Temporary buffer to store the old name
    char old_file_type[256];

    strncpy(old_file_type, app->file_type, sizeof(old_file_type) - 1);
    old_file_type[sizeof(old_file_type) - 1] = '\0'; // Null-terminate
    strncpy(app->file_type, app->temp_buffer_file_type, app->temp_buffer_size_file_type - 1);

    if(app->file_type_item) {
        variable_item_set_current_value_text(app->file_type_item, app->file_type);

        // Save the File Type to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);
    }

    rename_received_data(app->file_rename, app->file_rename, app->file_type, old_file_type);

    // set the file path for fhttp.file_path
    if(app->file_rename && app->file_type) {
        char file_path[256];
        snprintf(
            file_path,
            sizeof(file_path),
            "%s%s%s",
            RECEIVED_DATA_PATH,
            app->file_rename,
            app->file_type);
        file_path[sizeof(file_path) - 1] = '\0'; // Null-terminate
        strncpy(fhttp.file_path, file_path, sizeof(fhttp.file_path) - 1);
        fhttp.file_path[sizeof(fhttp.file_path) - 1] = '\0'; // Null-terminate
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListFile);
}

/**
 * @brief      Callback for when the user finishes entering the File Rename.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_file_rename_update(void* context) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->temp_buffer_file_rename || !app->temp_buffer_size_file_rename || !app->file_rename ||
       !app->file_rename_item) {
        FURI_LOG_E(TAG, "Invalid file rename buffer");
        return;
    }

    // Temporary buffer to store the old name
    char old_name[256];

    // Ensure that app->file_rename is null-terminated
    strncpy(old_name, app->file_rename, sizeof(old_name) - 1);
    old_name[sizeof(old_name) - 1] = '\0'; // Null-terminate

    // Store the entered File Rename from temp_buffer_file_rename to file_rename
    strncpy(app->file_rename, app->temp_buffer_file_rename, app->temp_buffer_size_file_rename - 1);

    if(app->file_rename_item) {
        variable_item_set_current_value_text(app->file_rename_item, app->file_rename);

        // Save the File Rename to the settings
        save_settings(
            app->path,
            app->ssid,
            app->password,
            app->file_rename,
            app->file_type,
            app->http_method,
            app->headers,
            app->payload);
    }

    rename_received_data(old_name, app->file_rename, app->file_type, app->file_type);

    // set the file path for fhttp.file_path
    if(app->file_rename && app->file_type) {
        char file_path[256];
        snprintf(
            file_path,
            sizeof(file_path),
            "%s%s%s",
            RECEIVED_DATA_PATH,
            app->file_rename,
            app->file_type);
        file_path[sizeof(file_path) - 1] = '\0'; // Null-terminate
        strncpy(fhttp.file_path, file_path, sizeof(fhttp.file_path) - 1);
        fhttp.file_path[sizeof(fhttp.file_path) - 1] = '\0'; // Null-terminate
    }

    // Return to the Configure view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewVariableItemListFile);
}

/**
 * @brief      Handler for Path configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_path_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    if(!app->text_input_path) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    UNUSED(index);

    // Initialize temp_buffer with existing path
    if(app->path && strlen(app->path) > 0) {
        strncpy(app->temp_buffer_path, app->path, app->temp_buffer_size_path - 1);
    } else {
        strncpy(app->temp_buffer_path, "https://httpbin.org/get", app->temp_buffer_size_path - 1);
    }

    app->temp_buffer_path[app->temp_buffer_size_path - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_path,
        web_crawler_set_path_updated,
        app,
        app->temp_buffer_path,
        app->temp_buffer_size_path,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_path), web_crawler_back_to_request_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInput);
}

/**
 * @brief      Handler for headers configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_headers_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_headers) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }
    if(!app->headers) {
        FURI_LOG_E(TAG, "Headers is NULL");
        return;
    }
    if(!app->temp_buffer_headers) {
        FURI_LOG_E(TAG, "Temp buffer headers is NULL");
        return;
    }

    // Initialize temp_buffer with existing headers
    if(app->headers && strlen(app->headers) > 0) {
        strncpy(app->temp_buffer_headers, app->headers, app->temp_buffer_size_headers - 1);
    } else {
        strncpy(
            app->temp_buffer_headers,
            "{\"Content-Type\":\"application/json\",\"key\":\"value\"}",
            app->temp_buffer_size_headers - 1);
    }

    app->temp_buffer_headers[app->temp_buffer_size_headers - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_headers,
        web_crawler_set_headers_updated,
        app,
        app->temp_buffer_headers,
        app->temp_buffer_size_headers,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_headers), web_crawler_back_to_request_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputHeaders);
}

/**
 * @brief      Handler for payload configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_payload_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_payload) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    // Initialize temp_buffer with existing payload
    if(app->payload && strlen(app->payload) > 0) {
        strncpy(app->temp_buffer_payload, app->payload, app->temp_buffer_size_payload - 1);
    } else {
        strncpy(
            app->temp_buffer_payload, "{\"key\":\"value\"}", app->temp_buffer_size_payload - 1);
    }

    app->temp_buffer_payload[app->temp_buffer_size_payload - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_payload,
        web_crawler_set_payload_updated,
        app,
        app->temp_buffer_payload,
        app->temp_buffer_size_payload,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_payload), web_crawler_back_to_request_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputPayload);
}

/**
 * @brief      Handler for SSID configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_ssid_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_ssid) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    // Initialize temp_buffer with existing SSID
    if(app->ssid && strlen(app->ssid) > 0) {
        strncpy(app->temp_buffer_ssid, app->ssid, app->temp_buffer_size_ssid - 1);
    } else {
        strncpy(app->temp_buffer_ssid, "", app->temp_buffer_size_ssid - 1);
    }

    app->temp_buffer_ssid[app->temp_buffer_size_ssid - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_ssid,
        web_crawler_set_ssid_updated,
        app,
        app->temp_buffer_ssid,
        app->temp_buffer_size_ssid,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_ssid), web_crawler_back_to_wifi_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputSSID);
}

/**
 * @brief      Handler for Password configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_password_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_password) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    // Initialize temp_buffer with existing password
    strncpy(app->temp_buffer_password, app->password, app->temp_buffer_size_password - 1);
    app->temp_buffer_password[app->temp_buffer_size_password - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_password,
        web_crawler_set_password_update,
        app,
        app->temp_buffer_password,
        app->temp_buffer_size_password,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_password), web_crawler_back_to_wifi_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputPassword);
}

/**
 * @brief      Handler for File Type configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_type_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_file_type) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    // Initialize temp_buffer with existing file_type
    if(app->file_type && strlen(app->file_type) > 0) {
        strncpy(app->temp_buffer_file_type, app->file_type, app->temp_buffer_size_file_type - 1);
    } else {
        strncpy(app->temp_buffer_file_type, ".txt", app->temp_buffer_size_file_type - 1);
    }

    app->temp_buffer_file_type[app->temp_buffer_size_file_type - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_file_type,
        web_crawler_set_file_type_update,
        app,
        app->temp_buffer_file_type,
        app->temp_buffer_size_file_type,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_file_type), web_crawler_back_to_file_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputFileType);
}

/**
 * @brief      Handler for File Rename configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_rename_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    if(!app->text_input_file_rename) {
        FURI_LOG_E(TAG, "Text input is NULL");
        return;
    }

    // Initialize temp_buffer with existing file_rename
    if(app->file_rename && strlen(app->file_rename) > 0) {
        strncpy(
            app->temp_buffer_file_rename, app->file_rename, app->temp_buffer_size_file_rename - 1);
    } else {
        strncpy(
            app->temp_buffer_file_rename, "received_data", app->temp_buffer_size_file_rename - 1);
    }

    app->temp_buffer_file_rename[app->temp_buffer_size_file_rename - 1] = '\0';

    // Configure the text input
    bool clear_previous_text = false;
    uart_text_input_set_result_callback(
        app->text_input_file_rename,
        web_crawler_set_file_rename_update,
        app,
        app->temp_buffer_file_rename,
        app->temp_buffer_size_file_rename,
        clear_previous_text);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_file_rename), web_crawler_back_to_file_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewTextInputFileRename);
}

/**
 * @brief      Handler for File Delete configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_delete_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);

    if(!delete_received_data(app)) {
        FURI_LOG_E(TAG, "Failed to delete file");
    }

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        widget_get_view(app->widget_file_delete), web_crawler_back_to_file_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewFileDelete);
}

void web_crawler_setting_item_file_read_clicked(void* context, uint32_t index) {
    WebCrawlerApp* app = (WebCrawlerApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "WebCrawlerApp is NULL");
        return;
    }
    UNUSED(index);
    widget_reset(app->widget_file_read);

    if(app->file_rename && app->file_type) {
        snprintf(
            fhttp.file_path,
            sizeof(fhttp.file_path),
            "%s%s%s",
            RECEIVED_DATA_PATH,
            app->file_rename,
            app->file_type);
    } else {
        snprintf(
            fhttp.file_path,
            sizeof(fhttp.file_path),
            "%s%s%s",
            RECEIVED_DATA_PATH,
            "received_data",
            ".txt");
    }

    // load the received data from the saved file
    FuriString* received_data = flipper_http_load_from_file(fhttp.file_path);
    if(received_data == NULL) {
        FURI_LOG_E(TAG, "Failed to load received data from file.");
        if(app->widget_file_read) {
            widget_add_text_scroll_element(app->widget_file_read, 0, 0, 128, 64, "File is empty.");
            view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewFileRead);
        }
        return;
    }
    const char* data_cstr = furi_string_get_cstr(received_data);
    if(data_cstr == NULL) {
        FURI_LOG_E(TAG, "Failed to get C-string from FuriString.");
        furi_string_free(received_data);
        if(app->widget_file_read) {
            widget_add_text_scroll_element(app->widget_file_read, 0, 0, 128, 64, "File is empty.");
            view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewFileRead);
        }
        return;
    }
    widget_add_text_scroll_element(app_instance->widget_file_read, 0, 0, 128, 64, data_cstr);
    furi_string_free(received_data);

    // Set the previous callback to return to Configure screen
    view_set_previous_callback(
        widget_get_view(app->widget_file_read), web_crawler_back_to_file_callback);

    // Show text input dialog
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewFileRead);
}
