#include "callback/flip_weather_callback.h"

bool weather_request_success = false;
bool sent_weather_request = false;
bool got_weather_data = false;

void flip_weather_popup_callback(void* context) {
    FlipWeatherApp* app = (FlipWeatherApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
        return;
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewSubmenu);
}

void flip_weather_request_error(Canvas* canvas) {
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
        } else if(
            strstr(fhttp.last_response, "[ERROR] GET request failed or returned empty data.") !=
            NULL) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "[ERROR] WiFi error.");
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
}

void flip_weather_handle_gps_draw(Canvas* canvas, bool show_gps_data) {
    if(sent_get_request) {
        if(fhttp.state == RECEIVING) {
            if(show_gps_data) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "Loading GPS...");
                canvas_draw_str(canvas, 0, 22, "Receiving...");
            }
        }
        // check status
        else if(fhttp.state == ISSUE || !get_request_success) {
            flip_weather_request_error(canvas);
        } else if(fhttp.state == IDLE) {
            // success, draw GPS
            process_geo_location();

            if(show_gps_data) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, city_data);
                canvas_draw_str(canvas, 0, 20, region_data);
                canvas_draw_str(canvas, 0, 30, country_data);
                canvas_draw_str(canvas, 0, 40, lat_data);
                canvas_draw_str(canvas, 0, 50, lon_data);
                canvas_draw_str(canvas, 0, 60, ip_data);
            }
        }
    }
}

// Callback for drawing the weather screen
void flip_weather_view_draw_callback_weather(Canvas* canvas, void* model) {
    if(!canvas) {
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

    canvas_draw_str(canvas, 0, 10, "Loading location data...");
    // handle geo location until it's processed and then handle weather

    // start the process
    if(!send_geo_location_request() || fhttp.state == ISSUE) {
        flip_weather_request_error(canvas);
    }
    // wait until geo location is processed
    if(!sent_get_request || !get_request_success || fhttp.state == RECEIVING) {
        canvas_draw_str(canvas, 0, 22, "Receiving data...");
        return;
    }
    // get/set geo lcoation once
    if(!geo_information_processed) {
        flip_weather_handle_gps_draw(canvas, false);
        canvas_draw_str(canvas, 0, 34, "Parsed location data.");
    }
    // start the weather process
    if(!sent_weather_request && fhttp.state == IDLE) {
        canvas_clear(canvas);
        canvas_draw_str(canvas, 0, 10, "Getting Weather...");
        sent_weather_request = true;
        char url[512];
        char* lattitude = lat_data + 10;
        char* longitude = lon_data + 11;
        char* headers = jsmn("Content-Type", "application/json");
        snprintf(
            url,
            512,
            "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current=temperature_2m,precipitation,rain,showers,snowfall&temperature_unit=celsius&wind_speed_unit=mph&precipitation_unit=inch&forecast_days=1",
            lattitude,
            longitude);
        weather_request_success = flipper_http_get_request_with_headers(url, headers);
        free(headers);
        if(!weather_request_success) {
            FURI_LOG_E(TAG, "Failed to send GET request");
            fhttp.state = ISSUE;
            flip_weather_request_error(canvas);
        }
        fhttp.state = RECEIVING;
    } else {
        if(fhttp.state == RECEIVING) {
            canvas_draw_str(canvas, 0, 10, "Loading Weather...");
            canvas_draw_str(canvas, 0, 22, "Receiving...");
            return;
        }

        // check status
        if(fhttp.state == ISSUE || !weather_request_success) {
            flip_weather_request_error(canvas);
            fhttp.state = ISSUE;
        } else {
            // success, draw weather
            process_weather();
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, temperature_data);
            canvas_draw_str(canvas, 0, 20, precipitation_data);
            canvas_draw_str(canvas, 0, 30, rain_data);
            canvas_draw_str(canvas, 0, 40, showers_data);
            canvas_draw_str(canvas, 0, 50, snowfall_data);
            canvas_draw_str(canvas, 0, 60, time_data);
        }
    }
}

// Callback for drawing the GPS screen
void flip_weather_view_draw_callback_gps(Canvas* canvas, void* model) {
    if(!canvas) {
        return;
    }
    UNUSED(model);

    if(fhttp.state == INACTIVE) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 7, "Wifi Dev Board disconnected.");
        canvas_draw_str(canvas, 0, 17, "Please connect to the board.");
        canvas_draw_str(canvas, 0, 32, "If your board is connected,");
        canvas_draw_str(canvas, 0, 42, "make sure you have flashed");
        canvas_draw_str(canvas, 0, 52, "your WiFi Devboard with the");
        canvas_draw_str(canvas, 0, 62, "latest FlipperHTTP flash.");
        return;
    }

    if(!send_geo_location_request() || fhttp.state == ISSUE) {
        flip_weather_request_error(canvas);
    }

    flip_weather_handle_gps_draw(canvas, true);
}

void callback_submenu_choices(void* context, uint32_t index) {
    FlipWeatherApp* app = (FlipWeatherApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
        return;
    }
    switch(index) {
    case FlipWeatherSubmenuIndexWeather:
        if(!flip_weather_handle_ip_address()) {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewPopupError);
        } else {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewWeather);
        }
        break;
    case FlipWeatherSubmenuIndexGPS:
        if(!flip_weather_handle_ip_address()) {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewPopupError);
        } else {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewGPS);
        }
        break;
    case FlipWeatherSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewAbout);
        break;
    case FlipWeatherSubmenuIndexSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewSettings);
        break;
    default:
        break;
    }
}

void text_updated_ssid(void* context) {
    FlipWeatherApp* app = (FlipWeatherApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
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
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewSettings);
}

void text_updated_password(void* context) {
    FlipWeatherApp* app = (FlipWeatherApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
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
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewSettings);
}

uint32_t callback_to_submenu(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    sent_get_request = false;
    get_request_success = false;
    got_ip_address = false;
    got_weather_data = false;
    geo_information_processed = false;
    weather_information_processed = false;
    sent_weather_request = false;
    weather_request_success = false;
    return FlipWeatherViewSubmenu;
}

void settings_item_selected(void* context, uint32_t index) {
    FlipWeatherApp* app = (FlipWeatherApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewTextInputSSID);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewTextInputPassword);
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

uint32_t callback_to_wifi_settings(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return FlipWeatherViewSettings;
}
