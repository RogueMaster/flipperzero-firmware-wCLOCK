#include <alloc/web_crawler_alloc.h>

/**
 * @brief      Function to allocate resources for the WebCrawlerApp.
 * @return     Pointer to the initialized WebCrawlerApp, or NULL on failure.
 */
WebCrawlerApp* web_crawler_app_alloc() {
    // Initialize the entire structure to zero to prevent undefined behavior
    WebCrawlerApp* app = (WebCrawlerApp*)malloc(sizeof(WebCrawlerApp));

    // Open GUI
    Gui* gui = furi_record_open(RECORD_GUI);

    // Initialize UART with the correct callback
    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize UART");
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // Allocate and initialize temp_buffer and path
    app->temp_buffer_size_path = 128;
    app->temp_buffer_size_ssid = 64;
    app->temp_buffer_size_password = 64;
    app->temp_buffer_size_file_type = 16;
    app->temp_buffer_size_file_rename = 128;
    app->temp_buffer_size_http_method = 16;
    app->temp_buffer_size_headers = 256;
    app->temp_buffer_size_payload = 256;
    if(!easy_flipper_set_buffer(&app->temp_buffer_path, app->temp_buffer_size_path)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->path, app->temp_buffer_size_path)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_ssid, app->temp_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->ssid, app->temp_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_password, app->temp_buffer_size_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->password, app->temp_buffer_size_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_file_type, app->temp_buffer_size_file_type)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->file_type, app->temp_buffer_size_file_type)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_file_rename, app->temp_buffer_size_file_rename)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->file_rename, app->temp_buffer_size_file_rename)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_http_method, app->temp_buffer_size_http_method)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->http_method, app->temp_buffer_size_http_method)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_headers, app->temp_buffer_size_headers)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->headers, app->temp_buffer_size_headers)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->temp_buffer_payload, app->temp_buffer_size_payload)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->payload, app->temp_buffer_size_payload)) {
        return NULL;
    }

    // Allocate TextInput views
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_path,
           WebCrawlerViewTextInput,
           "Enter URL",
           app->temp_buffer_path,
           app->temp_buffer_size_path,
           NULL,
           web_crawler_back_to_request_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_ssid,
           WebCrawlerViewTextInputSSID,
           "Enter SSID",
           app->temp_buffer_ssid,
           app->temp_buffer_size_ssid,
           NULL,
           web_crawler_back_to_wifi_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_password,
           WebCrawlerViewTextInputPassword,
           "Enter Password",
           app->temp_buffer_password,
           app->temp_buffer_size_password,
           NULL,
           web_crawler_back_to_wifi_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_file_type,
           WebCrawlerViewTextInputFileType,
           "Enter File Type",
           app->temp_buffer_file_type,
           app->temp_buffer_size_file_type,
           NULL,
           web_crawler_back_to_file_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_file_rename,
           WebCrawlerViewTextInputFileRename,
           "Enter File Rename",
           app->temp_buffer_file_rename,
           app->temp_buffer_size_file_rename,
           NULL,
           web_crawler_back_to_file_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_headers,
           WebCrawlerViewTextInputHeaders,
           "Enter Headers",
           app->temp_buffer_headers,
           app->temp_buffer_size_headers,
           NULL,
           web_crawler_back_to_request_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_payload,
           WebCrawlerViewTextInputPayload,
           "Enter Payload",
           app->temp_buffer_payload,
           app->temp_buffer_size_payload,
           NULL,
           web_crawler_back_to_request_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Allocate VariableItemList views
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_wifi,
           WebCrawlerViewVariableItemListWifi,
           web_crawler_wifi_enter_callback,
           web_crawler_back_to_configure_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_file,
           WebCrawlerViewVariableItemListFile,
           web_crawler_file_enter_callback,
           web_crawler_back_to_configure_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_request,
           WebCrawlerViewVariableItemListRequest,
           web_crawler_request_enter_callback,
           web_crawler_back_to_configure_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // set variable items
    app->path_item =
        variable_item_list_add(app->variable_item_list_request, "Path", 0, NULL, NULL);
    app->http_method_item = variable_item_list_add(
        app->variable_item_list_request, "HTTP Method", 5, web_crawler_http_method_change, app);
    app->headers_item =
        variable_item_list_add(app->variable_item_list_request, "Headers", 0, NULL, NULL);
    app->payload_item =
        variable_item_list_add(app->variable_item_list_request, "Payload", 0, NULL, NULL);
    //
    app->ssid_item =
        variable_item_list_add(app->variable_item_list_wifi, "SSID", 0, NULL, NULL); // index 0
    app->password_item =
        variable_item_list_add(app->variable_item_list_wifi, "Password", 0, NULL, NULL); // index 1
    //
    app->file_read_item = variable_item_list_add(
        app->variable_item_list_file, "Read File", 0, NULL, NULL); // index 0
    app->file_type_item = variable_item_list_add(
        app->variable_item_list_file, "Set File Type", 0, NULL, NULL); // index 1
    app->file_rename_item = variable_item_list_add(
        app->variable_item_list_file, "Rename File", 0, NULL, NULL); // index 2
    app->file_delete_item = variable_item_list_add(
        app->variable_item_list_file, "Delete File", 0, NULL, NULL); // index 3

    if(!app->ssid_item || !app->password_item || !app->file_type_item || !app->file_rename_item ||
       !app->path_item || !app->file_read_item || !app->file_delete_item ||
       !app->http_method_item || !app->headers_item || !app->payload_item) {
        free_all(app, "Failed to add items to VariableItemList");
        return NULL;
    }

    variable_item_set_current_value_text(app->path_item, ""); // Initialize
    variable_item_set_current_value_text(app->http_method_item, ""); // Initialize
    variable_item_set_current_value_text(app->headers_item, ""); // Initialize
    variable_item_set_current_value_text(app->payload_item, ""); // Initialize
    variable_item_set_current_value_text(app->ssid_item, ""); // Initialize
    variable_item_set_current_value_text(app->password_item, ""); // Initialize
    variable_item_set_current_value_text(app->file_type_item, ""); // Initialize
    variable_item_set_current_value_text(app->file_rename_item, ""); // Initialize
    variable_item_set_current_value_text(app->file_read_item, ""); // Initialize
    variable_item_set_current_value_text(app->file_delete_item, ""); // Initialize

    // Allocate Submenu views
    if(!easy_flipper_set_submenu(
           &app->submenu_main,
           WebCrawlerViewSubmenuMain,
           "Web Crawler v0.7",
           web_crawler_exit_app_callback,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_config,
           WebCrawlerViewSubmenuConfig,
           "Settings",
           web_crawler_back_to_main_callback,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Add Submenu items
    submenu_add_item(
        app->submenu_main, "Run", WebCrawlerSubmenuIndexRun, web_crawler_submenu_callback, app);
    submenu_add_item(
        app->submenu_main, "About", WebCrawlerSubmenuIndexAbout, web_crawler_submenu_callback, app);
    submenu_add_item(
        app->submenu_main,
        "Settings",
        WebCrawlerSubmenuIndexConfig,
        web_crawler_submenu_callback,
        app);

    submenu_add_item(
        app->submenu_config, "WiFi", WebCrawlerSubmenuIndexWifi, web_crawler_submenu_callback, app);
    submenu_add_item(
        app->submenu_config, "File", WebCrawlerSubmenuIndexFile, web_crawler_submenu_callback, app);
    submenu_add_item(
        app->submenu_config,
        "Request",
        WebCrawlerSubmenuIndexRequest,
        web_crawler_submenu_callback,
        app);

    // Allocate views
    if(!easy_flipper_set_view(
           &app->view_main,
           WebCrawlerViewMain,
           NULL,
           NULL,
           web_crawler_exit_app_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_run,
           WebCrawlerViewRun,
           web_crawler_view_draw_callback,
           NULL,
           web_crawler_back_to_main_callback,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    //-- WIDGET ABOUT VIEW --
    if(!easy_flipper_set_widget(
           &app->widget_about,
           WebCrawlerViewAbout,
           "Web Crawler App\n---\nThis is a web crawler app for Flipper Zero.\n---\nVisit github.com/jblanked for more details.\n---\nPress BACK to return.",
           web_crawler_back_to_main_callback,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_widget(
           &app->widget_file_read,
           WebCrawlerViewFileRead,
           "Data will be displayed here.",
           web_crawler_back_to_file_callback,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_widget(
           &app->widget_file_delete,
           WebCrawlerViewFileDelete,
           "File deleted.",
           web_crawler_back_to_file_callback,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Load Settings and Update Views
    if(!load_settings(
           app->path,
           app->temp_buffer_size_path,
           app->ssid,
           app->temp_buffer_size_ssid,
           app->password,
           app->temp_buffer_size_password,
           app->file_rename,
           app->temp_buffer_size_file_rename,
           app->file_type,
           app->temp_buffer_size_file_type,
           app->http_method,
           app->temp_buffer_size_http_method,
           app->headers,
           app->temp_buffer_size_headers,
           app->payload,
           app->temp_buffer_size_payload,
           app)) {
        FURI_LOG_E(TAG, "Failed to load settings");
    } else {
        // Update the configuration items based on loaded settings
        if(app->path_item) {
            variable_item_set_current_value_text(app->path_item, app->path);
        } else {
            variable_item_set_current_value_text(
                app->path_item, "https://httpbin.org/get"); // Initialize
        }

        if(app->ssid_item) {
            variable_item_set_current_value_text(app->ssid_item, app->ssid);
        } else {
            variable_item_set_current_value_text(app->ssid_item, ""); // Initialize
        }

        if(app->file_type_item) {
            variable_item_set_current_value_text(app->file_type_item, app->file_type);
        } else {
            variable_item_set_current_value_text(app->file_type_item, ".txt"); // Initialize
        }

        if(app->file_rename_item) {
            variable_item_set_current_value_text(app->file_rename_item, app->file_rename);
        } else {
            variable_item_set_current_value_text(
                app->file_rename_item, "received_data"); // Initialize
        }

        if(app->http_method_item) {
            variable_item_set_current_value_text(app->http_method_item, app->http_method);
        } else {
            variable_item_set_current_value_text(app->http_method_item, "GET"); // Initialize
        }

        if(app->headers_item) {
            variable_item_set_current_value_text(app->headers_item, app->headers);
        } else {
            variable_item_set_current_value_text(
                app->headers_item, "{\n\t\"Content-Type\": \"application/json\"\n}"); // Initialize
        }

        if(app->payload_item) {
            variable_item_set_current_value_text(app->payload_item, app->payload);
        } else {
            variable_item_set_current_value_text(
                app->payload_item, "{\n\t\"key\": \"value\"\n}"); // Initialize
        }

        // set the file path for fhttp.file_path
        char file_path[128];
        snprintf(
            file_path,
            sizeof(file_path),
            "%s%s%s",
            RECEIVED_DATA_PATH,
            app->file_rename,
            app->file_type);
        snprintf(fhttp.file_path, sizeof(fhttp.file_path), "%s", file_path);

        // update temp buffers
        strncpy(app->temp_buffer_path, app->path, app->temp_buffer_size_path - 1);
        app->temp_buffer_path[app->temp_buffer_size_path - 1] = '\0';
        strncpy(app->temp_buffer_ssid, app->ssid, app->temp_buffer_size_ssid - 1);
        app->temp_buffer_ssid[app->temp_buffer_size_ssid - 1] = '\0';
        strncpy(app->temp_buffer_file_type, app->file_type, app->temp_buffer_size_file_type - 1);
        app->temp_buffer_file_type[app->temp_buffer_size_file_type - 1] = '\0';
        strncpy(
            app->temp_buffer_file_rename, app->file_rename, app->temp_buffer_size_file_rename - 1);
        app->temp_buffer_file_rename[app->temp_buffer_size_file_rename - 1] = '\0';
        strncpy(
            app->temp_buffer_http_method, app->http_method, app->temp_buffer_size_http_method - 1);
        app->temp_buffer_http_method[app->temp_buffer_size_http_method - 1] = '\0';
        strncpy(app->temp_buffer_headers, app->headers, app->temp_buffer_size_headers - 1);
        app->temp_buffer_headers[app->temp_buffer_size_headers - 1] = '\0';
        strncpy(app->temp_buffer_payload, app->payload, app->temp_buffer_size_payload - 1);
        app->temp_buffer_payload[app->temp_buffer_size_payload - 1] = '\0';

        // Password handling can be omitted for security or handled securely
    }

    // Start with the Submenu view
    view_dispatcher_switch_to_view(app->view_dispatcher, WebCrawlerViewSubmenuMain);

    return app;
}
