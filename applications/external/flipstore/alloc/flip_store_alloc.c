#include <alloc/flip_store_alloc.h>
// Function to allocate resources for the FlipStoreApp
FlipStoreApp* flip_store_app_alloc() {
    FlipStoreApp* app = (FlipStoreApp*)malloc(sizeof(FlipStoreApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize flipper http");
        return NULL;
    }

    // Allocate the text input buffer
    app->uart_text_input_buffer_size_ssid = 64;
    app->uart_text_input_buffer_size_pass = 64;
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_pass, app->uart_text_input_buffer_size_pass)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_pass, app->uart_text_input_buffer_size_pass)) {
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // Main view
    if(!easy_flipper_set_view(
           &app->view_main,
           FlipStoreViewMain,
           flip_store_view_draw_callback_main,
           NULL,
           callback_to_app_list,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_app_info,
           FlipStoreViewAppInfo,
           flip_store_view_draw_callback_app_list,
           flip_store_input_callback,
           callback_to_app_list,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Widget
    if(!easy_flipper_set_widget(
           &app->widget,
           FlipStoreViewAbout,
           "Welcome to the FlipStore!\n------\nDownload apps via WiFi and\nrun them on your Flipper!\n------\nwww.github.com/jblanked",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Popup
    if(!easy_flipper_set_popup(
           &app->popup,
           FlipStoreViewPopup,
           "Failed",
           0,
           0,
           "You are not connected to Wifi.\n\nIf you have the FlipperHTTP\nflash installed, then update\nyour WiFi credentials.",
           0,
           10,
           popup_callback,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        FURI_LOG_E(TAG, "Failed to create popup");
    }

    // Dialog
    if(!easy_flipper_set_dialog_ex(
           &app->dialog_delete,
           FlipStoreViewAppDelete,
           "Delete App",
           0,
           0,
           "Are you sure you want to delete this app?",
           0,
           10,
           "No",
           "Yes",
           NULL,
           dialog_callback,
           callback_to_app_list,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Text Input
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_ssid,
           FlipStoreViewTextInputSSID,
           "Enter SSID",
           app->uart_text_input_temp_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           flip_store_text_updated_ssid,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_pass,
           FlipStoreViewTextInputPass,
           "Enter Password",
           app->uart_text_input_temp_buffer_pass,
           app->uart_text_input_buffer_size_pass,
           flip_store_text_updated_pass,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Variable Item List
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list,
           FlipStoreViewSettings,
           settings_item_selected,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    app->variable_item_ssid =
        variable_item_list_add(app->variable_item_list, "SSID", 0, NULL, NULL);
    app->variable_item_pass =
        variable_item_list_add(app->variable_item_list, "Password", 0, NULL, NULL);

    // Submenu
    if(!easy_flipper_set_submenu(
           &app->submenu_main,
           FlipStoreViewSubmenu,
           "FlipStore v0.5",
           callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_options,
           FlipStoreViewSubmenuOptions,
           "Browse",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list,
           FlipStoreViewAppList,
           "App Catalog",
           callback_to_submenu_options,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_firmwares,
           FlipStoreViewFirmwares,
           "ESP32 Firmwares",
           callback_to_submenu_options,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_bluetooth,
           FlipStoreViewAppListBluetooth,
           "Bluetooth",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_games,
           FlipStoreViewAppListGames,
           "Games",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_gpio,
           FlipStoreViewAppListGPIO,
           "GPIO",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_infrared,
           FlipStoreViewAppListInfrared,
           "Infrared",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_ibutton,
           FlipStoreViewAppListiButton,
           "iButton",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_media,
           FlipStoreViewAppListMedia,
           "Media",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_nfc,
           FlipStoreViewAppListNFC,
           "NFC",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_rfid,
           FlipStoreViewAppListRFID,
           "RFID",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_subghz,
           FlipStoreViewAppListSubGHz,
           "Sub-GHz",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_tools,
           FlipStoreViewAppListTools,
           "Tools",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_app_list_usb,
           FlipStoreViewAppListUSB,
           "USB",
           callback_to_app_list,
           &app->view_dispatcher)) {
        return NULL;
    }
    //
    submenu_add_item(
        app->submenu_main, "Browse", FlipStoreSubmenuIndexOptions, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu_main, "About", FlipStoreSubmenuIndexAbout, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu_main,
        "Settings",
        FlipStoreSubmenuIndexSettings,
        callback_submenu_choices,
        app);
    //
    submenu_add_item(
        app->submenu_options,
        "App Catalog",
        FlipStoreSubmenuIndexAppList,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_options,
        "ESP32 Firmwares",
        FlipStoreSubmenuIndexFirmwares,
        callback_submenu_choices,
        app);
    //
    for(int i = 0; i < 3; i++) {
        submenu_add_item(
            app->submenu_firmwares,
            firmwares[i],
            FlipStoreSubmenuIndexStartFirmwares + i,
            callback_submenu_choices,
            app);
    }
    //
    submenu_add_item(
        app->submenu_app_list,
        "Bluetooth",
        FlipStoreSubmenuIndexAppListBluetooth,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "Games",
        FlipStoreSubmenuIndexAppListGames,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "GPIO",
        FlipStoreSubmenuIndexAppListGPIO,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "Infrared",
        FlipStoreSubmenuIndexAppListInfrared,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "iButton",
        FlipStoreSubmenuIndexAppListiButton,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "Media",
        FlipStoreSubmenuIndexAppListMedia,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "NFC",
        FlipStoreSubmenuIndexAppListNFC,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "RFID",
        FlipStoreSubmenuIndexAppListRFID,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "Sub-GHz",
        FlipStoreSubmenuIndexAppListSubGHz,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "Tools",
        FlipStoreSubmenuIndexAppListTools,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_app_list,
        "USB",
        FlipStoreSubmenuIndexAppListUSB,
        callback_submenu_choices,
        app);
    //
    // dont add any items to the app list submenu of each category yet

    // load settings
    if(load_settings(
           app->uart_text_input_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           app->uart_text_input_buffer_pass,
           app->uart_text_input_buffer_size_pass)) {
        // Update variable items
        if(app->variable_item_ssid)
            variable_item_set_current_value_text(
                app->variable_item_ssid, app->uart_text_input_buffer_ssid);
        // do not display the password

        // Copy items into their temp buffers with safety checks
        if(app->uart_text_input_buffer_ssid && app->uart_text_input_temp_buffer_ssid) {
            strncpy(
                app->uart_text_input_temp_buffer_ssid,
                app->uart_text_input_buffer_ssid,
                app->uart_text_input_buffer_size_ssid - 1);
            app->uart_text_input_temp_buffer_ssid[app->uart_text_input_buffer_size_ssid - 1] =
                '\0';
        }
        if(app->uart_text_input_buffer_pass && app->uart_text_input_temp_buffer_pass) {
            strncpy(
                app->uart_text_input_temp_buffer_pass,
                app->uart_text_input_buffer_pass,
                app->uart_text_input_buffer_size_pass - 1);
            app->uart_text_input_temp_buffer_pass[app->uart_text_input_buffer_size_pass - 1] =
                '\0';
        }
    }

    // Switch to the main view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewSubmenu);

    return app;
}
