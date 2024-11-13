#include <alloc/flip_wifi_alloc.h>

// Function to allocate resources for the FlipWiFiApp
FlipWiFiApp* flip_wifi_app_alloc() {
    FlipWiFiApp* app = (FlipWiFiApp*)malloc(sizeof(FlipWiFiApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    // initialize uart
    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize flipper http");
        return NULL;
    }

    // Allocate the text input buffer
    app->uart_text_input_buffer_size_password_scan = 64;
    app->uart_text_input_buffer_size_password_saved = 64;
    app->uart_text_input_buffer_size_add_ssid = 64;
    app->uart_text_input_buffer_size_add_password = 64;
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_password_scan,
           app->uart_text_input_buffer_size_password_scan)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_password_scan,
           app->uart_text_input_buffer_size_password_scan)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_password_saved,
           app->uart_text_input_buffer_size_password_saved)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_password_saved,
           app->uart_text_input_buffer_size_password_saved)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_add_ssid, app->uart_text_input_buffer_size_add_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_add_ssid,
           app->uart_text_input_buffer_size_add_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_add_password,
           app->uart_text_input_buffer_size_add_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_add_password,
           app->uart_text_input_buffer_size_add_password)) {
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // View(s)
    if(!easy_flipper_set_view(
           &app->view_wifi_scan,
           FlipWiFiViewWiFiScan,
           flip_wifi_view_draw_callback_scan,
           flip_wifi_view_input_callback_scan,
           callback_to_submenu_scan,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_wifi_saved,
           FlipWiFiViewWiFiSaved,
           flip_wifi_view_draw_callback_saved,
           flip_wifi_view_input_callback_saved,
           callback_to_submenu_saved,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Widget
    if(!easy_flipper_set_widget(
           &app->widget_info,
           FlipWiFiViewAbout,
           "FlipWiFi v1.1\n-----\nFlipperHTTP companion app.\nScan and save WiFi networks.\n-----\nwww.github.com/jblanked",
           callback_to_submenu_main,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Text Input
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_password_scan,
           FlipWiFiViewTextInputScan,
           "Enter WiFi Password",
           app->uart_text_input_temp_buffer_password_scan,
           app->uart_text_input_buffer_size_password_scan,
           flip_wifi_text_updated_password_scan,
           callback_to_submenu_scan,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_password_saved,
           FlipWiFiViewTextInputSaved,
           "Enter WiFi Password",
           app->uart_text_input_temp_buffer_password_saved,
           app->uart_text_input_buffer_size_password_saved,
           flip_wifi_text_updated_password_saved,
           callback_to_submenu_saved,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_add_ssid,
           FlipWiFiViewTextInputSavedAddSSID,
           "Enter SSID",
           app->uart_text_input_temp_buffer_add_ssid,
           app->uart_text_input_buffer_size_add_ssid,
           flip_wifi_text_updated_add_ssid,
           callback_to_submenu_saved,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_add_password,
           FlipWiFiViewTextInputSavedAddPassword,
           "Enter Password",
           app->uart_text_input_temp_buffer_add_password,
           app->uart_text_input_buffer_size_add_password,
           flip_wifi_text_updated_add_password,
           callback_to_submenu_saved,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Submenu
    if(!easy_flipper_set_submenu(
           &app->submenu_main,
           FlipWiFiViewSubmenuMain,
           "FlipWiFi v1.1",
           easy_flipper_callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_wifi_scan,
           FlipWiFiViewSubmenuScan,
           "WiFi Scan",
           callback_to_submenu_main,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_wifi_saved,
           FlipWiFiViewSubmenuSaved,
           "Saved APs",
           callback_to_submenu_main,
           &app->view_dispatcher)) {
        return NULL;
    }
    submenu_add_item(
        app->submenu_main, "Scan", FlipWiFiSubmenuIndexWiFiScan, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu_main,
        "Saved APs",
        FlipWiFiSubmenuIndexWiFiSaved,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_main, "Info", FlipWiFiSubmenuIndexAbout, callback_submenu_choices, app);

    // Popup
    if(!easy_flipper_set_popup(
           &app->popup,
           FlipWiFiViewPopup,
           "Success",
           0,
           0,
           "The WiFi setting has been set.",
           0,
           10,
           popup_callback_saved,
           callback_to_submenu_saved,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Load the playlist from storage
    if(!load_playlist(&app->wifi_playlist)) {
        FURI_LOG_E(TAG, "Failed to load playlist");

        // playlist is empty?
        submenu_reset(app->submenu_wifi_saved);
        submenu_set_header(app->submenu_wifi_saved, "Saved APs");
        submenu_add_item(
            app->submenu_wifi_saved,
            "[Add Network]",
            FlipWiFiSubmenuIndexWiFiSavedAddSSID,
            callback_submenu_choices,
            app);
    } else {
        // Update the submenu
        flip_wifi_redraw_submenu_saved(app);
    }

    // Switch to the main view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipWiFiViewSubmenuMain);

    return app;
}
