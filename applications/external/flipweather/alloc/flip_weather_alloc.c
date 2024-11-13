#include <alloc/flip_weather_alloc.h>

// Function to allocate resources for the FlipWeatherApp
FlipWeatherApp* flip_weather_app_alloc() {
    FlipWeatherApp* app = (FlipWeatherApp*)malloc(sizeof(FlipWeatherApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    // initialize uart
    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize flipper http");
        return NULL;
    }

    // Allocate the text input buffer
    app->uart_text_input_buffer_size_ssid = 64;
    app->uart_text_input_buffer_size_password = 64;
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_password, app->uart_text_input_buffer_size_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_password,
           app->uart_text_input_buffer_size_password)) {
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // Main view
    if(!easy_flipper_set_view(
           &app->view_weather,
           FlipWeatherViewWeather,
           flip_weather_view_draw_callback_weather,
           NULL,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_gps,
           FlipWeatherViewGPS,
           flip_weather_view_draw_callback_gps,
           NULL,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Widget
    if(!easy_flipper_set_widget(
           &app->widget,
           FlipWeatherViewAbout,
           "FlipWeather v1.1\n-----\nUse WiFi to get GPS and \nWeather information.\n-----\nwww.github.com/jblanked",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Text Input
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_ssid,
           FlipWeatherViewTextInputSSID,
           "Enter SSID",
           app->uart_text_input_temp_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           text_updated_ssid,
           callback_to_wifi_settings,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_password,
           FlipWeatherViewTextInputPassword,
           "Enter Password",
           app->uart_text_input_temp_buffer_password,
           app->uart_text_input_buffer_size_password,
           text_updated_password,
           callback_to_wifi_settings,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Variable Item List
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list,
           FlipWeatherViewSettings,
           settings_item_selected,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    app->variable_item_ssid =
        variable_item_list_add(app->variable_item_list, "SSID", 0, NULL, NULL);
    app->variable_item_password =
        variable_item_list_add(app->variable_item_list, "Password", 0, NULL, NULL);
    variable_item_set_current_value_text(app->variable_item_ssid, "");
    variable_item_set_current_value_text(app->variable_item_password, "");

    // Submenu
    if(!easy_flipper_set_submenu(
           &app->submenu,
           FlipWeatherViewSubmenu,
           "FlipWeather v1.1",
           callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    submenu_add_item(
        app->submenu, "Weather", FlipWeatherSubmenuIndexWeather, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu, "GPS", FlipWeatherSubmenuIndexGPS, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu, "About", FlipWeatherSubmenuIndexAbout, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu, "Settings", FlipWeatherSubmenuIndexSettings, callback_submenu_choices, app);

    // load settings
    if(load_settings(
           app->uart_text_input_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           app->uart_text_input_buffer_password,
           app->uart_text_input_buffer_size_password)) {
        // Update variable items
        if(app->variable_item_ssid)
            variable_item_set_current_value_text(
                app->variable_item_ssid, app->uart_text_input_buffer_ssid);
        // dont show password

        // Copy items into their temp buffers with safety checks
        if(app->uart_text_input_buffer_ssid && app->uart_text_input_temp_buffer_ssid) {
            strncpy(
                app->uart_text_input_temp_buffer_ssid,
                app->uart_text_input_buffer_ssid,
                app->uart_text_input_buffer_size_ssid - 1);
            app->uart_text_input_temp_buffer_ssid[app->uart_text_input_buffer_size_ssid - 1] =
                '\0';
        }
        if(app->uart_text_input_buffer_password && app->uart_text_input_temp_buffer_password) {
            strncpy(
                app->uart_text_input_temp_buffer_password,
                app->uart_text_input_buffer_password,
                app->uart_text_input_buffer_size_password - 1);
            app->uart_text_input_temp_buffer_password[app->uart_text_input_buffer_size_password - 1] =
                '\0';
        }
    }

    // Popup
    if(!easy_flipper_set_popup(
           &app->popup_error,
           FlipWeatherViewPopupError,
           "[ERROR]",
           0,
           0,
           "Wifi Dev Board disconnected.\nPlease connect to the board.\nIf your board is connected,\nmake sure you have flashed\nyour WiFi Devboard with the\nlatest FlipperHTTP flash.",
           0,
           7,
           flip_weather_popup_callback,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Switch to the main view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWeatherViewSubmenu);

    return app;
}
