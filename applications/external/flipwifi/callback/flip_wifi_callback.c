#include <callback/flip_wifi_callback.h>

char* ssid_list[64];
uint32_t ssid_index = 0;

void flip_wifi_redraw_submenu_saved(FlipWiFiApp* app) {
    // re draw the saved submenu
    submenu_reset(app->submenu_wifi_saved);
    submenu_set_header(app->submenu_wifi_saved, "Saved APs");
    submenu_add_item(
        app->submenu_wifi_saved,
        "[Add Network]",
        FlipWiFiSubmenuIndexWiFiSavedAddSSID,
        callback_submenu_choices,
        app);
    for(uint32_t i = 0; i < app->wifi_playlist.count; i++) {
        submenu_add_item(
            app->submenu_wifi_saved,
            app->wifi_playlist.ssids[i],
            FlipWiFiSubmenuIndexWiFiSavedStart + i,
            callback_submenu_choices,
            app);
    }
}

uint32_t callback_to_submenu_main(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    ssid_index = 0;
    return FlipWiFiViewSubmenuMain;
}
uint32_t callback_to_submenu_scan(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    ssid_index = 0;
    return FlipWiFiViewSubmenuScan;
}
uint32_t callback_to_submenu_saved(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    ssid_index = 0;
    return FlipWiFiViewSubmenuSaved;
}
void popup_callback_saved(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "HelloWorldApp is NULL");
        return;
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
}
void popup_callback_main(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "HelloWorldApp is NULL");
        return;
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuMain);
}

// Callback for drawing the main screen
void flip_wifi_view_draw_callback_scan(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, ssid_list[ssid_index]);
    canvas_draw_icon(canvas, 0, 53, &I_ButtonBACK_10x8);
    canvas_draw_str_aligned(canvas, 12, 54, AlignLeft, AlignTop, "Back");
    canvas_draw_icon(canvas, 96, 53, &I_ButtonRight_4x7);
    canvas_draw_str_aligned(canvas, 103, 54, AlignLeft, AlignTop, "Add");
}
void flip_wifi_view_draw_callback_saved(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, app_instance->wifi_playlist.ssids[ssid_index]);
    canvas_set_font(canvas, FontSecondary);
    char password[64];
    snprintf(
        password, sizeof(password), "Pass: %s", app_instance->wifi_playlist.passwords[ssid_index]);
    canvas_draw_str(canvas, 0, 20, password);
    canvas_draw_icon(canvas, 0, 54, &I_ButtonLeft_4x7);
    canvas_draw_str_aligned(canvas, 7, 54, AlignLeft, AlignTop, "Delete");
    canvas_draw_icon(canvas, 37, 53, &I_ButtonBACK_10x8);
    canvas_draw_str_aligned(canvas, 49, 54, AlignLeft, AlignTop, "Back");
    canvas_draw_icon(canvas, 73, 54, &I_ButtonOK_7x7);
    canvas_draw_str_aligned(canvas, 81, 54, AlignLeft, AlignTop, "Set");
    canvas_draw_icon(canvas, 100, 54, &I_ButtonRight_4x7);
    canvas_draw_str_aligned(canvas, 107, 54, AlignLeft, AlignTop, "Edit");
}

// Input callback for the view (async input handling)
bool flip_wifi_view_input_callback_scan(InputEvent* event, void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(event->type == InputTypePress && event->key == InputKeyRight) {
        // switch to text input to set password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewTextInputScan);
        return true;
    }
    return false;
}
// Input callback for the view (async input handling)
bool flip_wifi_view_input_callback_saved(InputEvent* event, void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return false;
    }
    if(event->type == InputTypePress && event->key == InputKeyRight) {
        // set text input buffer as the selected password
        strncpy(
            app->uart_text_input_temp_buffer_password_saved,
            app->wifi_playlist.passwords[ssid_index],
            app->uart_text_input_buffer_size_password_saved);
        // switch to text input to set password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewTextInputSaved);
        return true;
    } else if(event->type == InputTypePress && event->key == InputKeyOk) {
        // save the settings
        if(app->wifi_playlist.ssids[ssid_index] == NULL ||
           app->wifi_playlist.passwords[ssid_index] == NULL) {
            return false;
        }
        save_settings(
            app->wifi_playlist.ssids[ssid_index], app->wifi_playlist.passwords[ssid_index]);

        flipper_http_save_wifi(
            app->wifi_playlist.ssids[ssid_index], app->wifi_playlist.passwords[ssid_index]);

        flipper_http_connect_wifi();

        popup_set_header(app->popup, "[SUCCESS]", 0, 0, AlignLeft, AlignTop);
        popup_set_text(
            app->popup,
            "All FlipperHTTP apps will now\nuse the selected network.",
            0,
            40,
            AlignLeft,
            AlignTop);
        view_set_previous_callback(popup_get_view(app->popup), callback_to_submenu_saved);
        popup_set_callback(app->popup, popup_callback_saved);

        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewPopup);
        return true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        // delete the selected ssid and password
        free(app->wifi_playlist.ssids[ssid_index]);
        free(app->wifi_playlist.passwords[ssid_index]);
        free(ssid_list[ssid_index]);
        // shift the remaining ssids and passwords
        for(uint32_t i = ssid_index; i < app->wifi_playlist.count - 1; i++) {
            app->wifi_playlist.ssids[i] = app->wifi_playlist.ssids[i + 1];
            app->wifi_playlist.passwords[i] = app->wifi_playlist.passwords[i + 1];
            ssid_list[i] = ssid_list[i + 1];
        }
        app->wifi_playlist.count--;

        // save the playlist to storage
        save_playlist(&app->wifi_playlist);

        // re draw the saved submenu
        flip_wifi_redraw_submenu_saved(app);
        // switch back to the saved view
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
        return true;
    }
    return false;
}

// Function to trim leading and trailing whitespace
// Returns the trimmed start pointer and updates the length
char* trim_whitespace(char* start, size_t* length) {
    // Trim leading whitespace
    while(*length > 0 && isspace((unsigned char)*start)) {
        start++;
        (*length)--;
    }

    // Trim trailing whitespace
    while(*length > 0 && isspace((unsigned char)start[*length - 1])) {
        (*length)--;
    }

    return start;
}

bool flip_wifi_handle_scan(FlipWiFiApp* app) {
    // load the received data from the saved file
    FuriString* scan_data = flipper_http_load_from_file(fhttp.file_path);
    if(scan_data == NULL) {
        FURI_LOG_E(TAG, "Failed to load received data from file.");
        fhttp.state = ISSUE;
        return false;
    }
    char* data_cstr = (char*)furi_string_get_cstr(scan_data);
    if(data_cstr == NULL) {
        FURI_LOG_E(TAG, "Failed to get C-string from FuriString.");
        furi_string_free(scan_data);
        fhttp.state = ISSUE;
        free(data_cstr);
        return false;
    }

    uint32_t ssid_count = 0;

    char* current_position = data_cstr;
    char* next_comma = NULL;

    // Manually split the string on commas
    while((next_comma = strchr(current_position, ',')) != NULL) {
        // Calculate length of the SSID
        size_t ssid_length = next_comma - current_position;

        // Trim leading and trailing whitespace
        size_t trimmed_length = ssid_length;
        char* trim_start = trim_whitespace(current_position, &trimmed_length);

        // Handle empty SSIDs resulting from consecutive commas
        if(trimmed_length == 0) {
            current_position = next_comma + 1; // Move past the comma
            continue;
        }

        // Allocate memory for the SSID and copy it
        ssid_list[ssid_count] = malloc(trimmed_length + 1);
        if(ssid_list[ssid_count] == NULL) {
            FURI_LOG_E(TAG, "Memory allocation failed");
            free(data_cstr);
            furi_string_free(scan_data);
            return false;
        }
        strncpy(ssid_list[ssid_count], trim_start, trimmed_length);
        ssid_list[ssid_count][trimmed_length] = '\0'; // Null-terminate the string

        ssid_count++;
        if(ssid_count >= MAX_WIFI_NETWORKS) {
            FURI_LOG_E(TAG, "Maximum SSID limit reached");
            break;
        }

        current_position = next_comma + 1; // Move past the comma
    }

    // Handle the last SSID after the last comma (if any)
    if(*current_position != '\0' && ssid_count < MAX_WIFI_NETWORKS) {
        size_t ssid_length = strlen(current_position);

        // Trim leading and trailing whitespace
        size_t trimmed_length = ssid_length;
        char* trim_start = trim_whitespace(current_position, &trimmed_length);

        // Handle empty SSIDs
        if(trimmed_length > 0) {
            ssid_list[ssid_count] = malloc(trimmed_length + 1);
            if(ssid_list[ssid_count] == NULL) {
                FURI_LOG_E(TAG, "Memory allocation failed for the last SSID");
                return false;
            }
            strncpy(ssid_list[ssid_count], trim_start, trimmed_length);
            ssid_list[ssid_count][trimmed_length] = '\0'; // Null-terminate the string
            ssid_count++;
        }
    }

    // Add each SSID as a submenu item
    submenu_set_header(app->submenu_wifi_scan, "WiFi Nearby");
    for(uint32_t i = 0; i < ssid_count; i++) {
        char* ssid_item = ssid_list[i];
        if(ssid_item == NULL) {
            // skip any NULL entries
            continue;
        }
        char ssid[64];
        snprintf(ssid, sizeof(ssid), "%s", ssid_item);
        submenu_add_item(
            app->submenu_wifi_scan,
            ssid,
            FlipWiFiSubmenuIndexWiFiScanStart + i,
            callback_submenu_choices,
            app);
    }
    free(data_cstr);
    furi_string_free(scan_data);
    return true;
}
void callback_submenu_choices(void* context, uint32_t index) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }
    switch(index) {
    case FlipWiFiSubmenuIndexWiFiScan:
        if(fhttp.state == INACTIVE) {
            popup_set_header(app->popup, "[ERROR]", 0, 0, AlignLeft, AlignTop);
            popup_set_text(
                app->popup,
                "WiFi Devboard Disconnected.\nPlease reconnect the board.",
                0,
                40,
                AlignLeft,
                AlignTop);
            view_set_previous_callback(popup_get_view(app->popup), callback_to_submenu_main);
            popup_set_callback(app->popup, popup_callback_main);
            // switch to the popup view
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewPopup);
        }
        // scan for wifi
        if(!flipper_http_scan_wifi()) {
            FURI_LOG_E(TAG, "Failed to scan for WiFi");
            fhttp.state = ISSUE;
            return;
        } else // start the async feed request
        {
            furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
            fhttp.state = RECEIVING;
        }
        while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
            // Wait for the feed to be received
            furi_delay_ms(100);
        }
        furi_timer_stop(fhttp.get_timeout_timer);
        // set each SSID as a submenu item
        if(fhttp.state != IDLE) {
            fhttp.state = ISSUE;
            FURI_LOG_E(TAG, "Failed to receive WiFi scan");
            popup_set_header(app->popup, "[ERROR]", 0, 0, AlignLeft, AlignTop);
            popup_set_text(
                app->popup,
                "Failed to scan...\nTry reconnecting the board!",
                0,
                40,
                AlignLeft,
                AlignTop);
            view_set_previous_callback(popup_get_view(app->popup), callback_to_submenu_main);
            popup_set_callback(app->popup, popup_callback_main);
            // switch to the popup view
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewPopup);
        } else {
            submenu_reset(app->submenu_wifi_scan);
            submenu_set_header(app->submenu_wifi_scan, "WiFi Nearby");
            if(flip_wifi_handle_scan(app)) {
                // switch to the submenu
                view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuScan);
            } else {
                fhttp.state = ISSUE;
                FURI_LOG_E(TAG, "Failed to handle WiFi scan");
                popup_set_header(app->popup, "[ERROR]", 0, 0, AlignLeft, AlignTop);
                popup_set_text(
                    app->popup,
                    "Failed to scan...\nTry reconnecting the board!",
                    0,
                    40,
                    AlignLeft,
                    AlignTop);
                view_set_previous_callback(popup_get_view(app->popup), callback_to_submenu_main);
                popup_set_callback(app->popup, popup_callback_main);
                // switch to the popup view
                view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewPopup);
            }
            return;
        }
        break;
    case FlipWiFiSubmenuIndexWiFiSaved:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
        break;
    case FlipWiFiSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewAbout);
        break;
    case FlipWiFiSubmenuIndexWiFiSavedAddSSID:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewTextInputSavedAddSSID);
        break;
    case 100 ... 163:
        ssid_index = index - FlipWiFiSubmenuIndexWiFiScanStart;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewWiFiScan);
        break;
    case 200 ... 263:
        ssid_index = index - FlipWiFiSubmenuIndexWiFiSavedStart;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewWiFiSaved);
        break;
    default:
        break;
    }
}

void flip_wifi_text_updated_password_scan(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_password_scan,
        app->uart_text_input_temp_buffer_password_scan,
        app->uart_text_input_buffer_size_password_scan);

    // Ensure null-termination
    app->uart_text_input_buffer_password_scan[app->uart_text_input_buffer_size_password_scan - 1] =
        '\0';

    // add the SSID and password_scan to the playlist
    app->wifi_playlist.ssids[app->wifi_playlist.count] = strdup(ssid_list[ssid_index]);
    app->wifi_playlist.passwords[app->wifi_playlist.count] =
        strdup(app->uart_text_input_buffer_password_scan);
    app->wifi_playlist.count++;

    // save the playlist to storage
    save_playlist(&app->wifi_playlist);

    flip_wifi_redraw_submenu_saved(app);

    // switch to back to the scan view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuScan);
}
void flip_wifi_text_updated_password_saved(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_password_saved,
        app->uart_text_input_temp_buffer_password_saved,
        app->uart_text_input_buffer_size_password_saved);

    // Ensure null-termination
    app->uart_text_input_buffer_password_saved[app->uart_text_input_buffer_size_password_saved - 1] =
        '\0';

    // update the password_saved in the playlist
    app->wifi_playlist.passwords[ssid_index] = strdup(app->uart_text_input_buffer_password_saved);

    // save the playlist to storage
    save_playlist(&app->wifi_playlist);

    // switch to back to the saved view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
}

void flip_wifi_text_updated_add_ssid(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_add_ssid,
        app->uart_text_input_temp_buffer_add_ssid,
        app->uart_text_input_buffer_size_add_ssid);

    // Ensure null-termination
    app->uart_text_input_buffer_add_ssid[app->uart_text_input_buffer_size_add_ssid - 1] = '\0';

    // do nothing for now, go to the next text input to set the password
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewTextInputSavedAddPassword);
}
void flip_wifi_text_updated_add_password(void* context) {
    FlipWiFiApp* app = (FlipWiFiApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_add_password,
        app->uart_text_input_temp_buffer_add_password,
        app->uart_text_input_buffer_size_add_password);

    // Ensure null-termination
    app->uart_text_input_buffer_add_password[app->uart_text_input_buffer_size_add_password - 1] =
        '\0';

    // add the SSID and password_scan to the playlist
    app->wifi_playlist.ssids[app->wifi_playlist.count] =
        strdup(app->uart_text_input_buffer_add_ssid);
    app->wifi_playlist.passwords[app->wifi_playlist.count] =
        strdup(app->uart_text_input_buffer_add_password);
    app->wifi_playlist.count++;

    // save the playlist to storage
    save_playlist(&app->wifi_playlist);

    flip_wifi_redraw_submenu_saved(app);

    // switch to back to the saved view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
}
