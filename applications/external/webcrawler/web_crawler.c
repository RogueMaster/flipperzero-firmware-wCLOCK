#include <web_crawler.h>

void free_buffers(WebCrawlerApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "Invalid app context");
        return;
    }
    if(app->path) {
        free(app->path);
        app->path = NULL;
    }

    if(app->temp_buffer_path) {
        free(app->temp_buffer_path);
        app->temp_buffer_path = NULL;
    }

    if(app->ssid) {
        free(app->ssid);
        app->ssid = NULL;
    }

    if(app->temp_buffer_ssid) {
        free(app->temp_buffer_ssid);
        app->temp_buffer_ssid = NULL;
    }

    if(app->password) {
        free(app->password);
        app->password = NULL;
    }

    if(app->temp_buffer_password) {
        free(app->temp_buffer_password);
        app->temp_buffer_password = NULL;
    }

    if(app->file_type) {
        free(app->file_type);
        app->file_type = NULL;
    }

    if(app->temp_buffer_file_type) {
        free(app->temp_buffer_file_type);
        app->temp_buffer_file_type = NULL;
    }

    if(app->file_rename) {
        free(app->file_rename);
        app->file_rename = NULL;
    }

    if(app->temp_buffer_file_rename) {
        free(app->temp_buffer_file_rename);
        app->temp_buffer_file_rename = NULL;
    }

    if(app->temp_buffer_http_method) {
        free(app->temp_buffer_http_method);
        app->temp_buffer_http_method = NULL;
    }

    if(app->temp_buffer_headers) {
        free(app->temp_buffer_headers);
        app->temp_buffer_headers = NULL;
    }

    if(app->temp_buffer_payload) {
        free(app->temp_buffer_payload);
        app->temp_buffer_payload = NULL;
    }

    if(app->http_method) {
        free(app->http_method);
        app->http_method = NULL;
    }

    if(app->headers) {
        free(app->headers);
        app->headers = NULL;
    }

    if(app->payload) {
        free(app->payload);
        app->payload = NULL;
    }
}

void free_resources(WebCrawlerApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "Invalid app context");
        return;
    }

    free_buffers(app);
}

void free_all(WebCrawlerApp* app, char* reason) {
    if(!app) {
        FURI_LOG_E(TAG, "Invalid app context");
        return;
    }
    if(reason) {
        FURI_LOG_I(TAG, reason);
    }

    if(app->view_main) view_free(app->view_main);
    if(app->submenu_main) submenu_free(app->submenu_main);
    if(app->submenu_config) submenu_free(app->submenu_config);
    if(app->variable_item_list_wifi) variable_item_list_free(app->variable_item_list_wifi);
    if(app->variable_item_list_file) variable_item_list_free(app->variable_item_list_file);
    if(app->variable_item_list_request) variable_item_list_free(app->variable_item_list_request);
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);
    if(app->widget_about) widget_free(app->widget_about);
    if(app->widget_file_read) widget_free(app->widget_file_read);
    if(app->widget_file_delete) widget_free(app->widget_file_delete);
    if(app->text_input_path) uart_text_input_free(app->text_input_path);
    if(app->text_input_ssid) uart_text_input_free(app->text_input_ssid);
    if(app->text_input_password) uart_text_input_free(app->text_input_password);
    if(app->text_input_file_type) uart_text_input_free(app->text_input_file_type);
    if(app->text_input_file_rename) uart_text_input_free(app->text_input_file_rename);
    if(app->text_input_headers) uart_text_input_free(app->text_input_headers);
    if(app->text_input_payload) uart_text_input_free(app->text_input_payload);

    furi_record_close(RECORD_GUI);
    free_resources(app);
}
/**
 * @brief      Function to free the resources used by WebCrawlerApp.
 * @param      app  The WebCrawlerApp object to free.
 */
void web_crawler_app_free(WebCrawlerApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "Invalid app context");
        return;
    }

    if(!app->view_dispatcher) {
        FURI_LOG_E(TAG, "Invalid view dispatcher");
        return;
    }

    // Remove and free Main view
    if(app->view_main) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewMain);
        view_free(app->view_main);
    }
    if(app->view_run) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewRun);
        view_free(app->view_run);
    }
    // Deinitialize UART
    flipper_http_deinit();

    // Remove and free Submenu
    if(app->submenu_main) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewSubmenuMain);
        submenu_free(app->submenu_main);
    }
    if(app->submenu_config) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewSubmenuConfig);
        submenu_free(app->submenu_config);
    }
    // Remove and free Variable Item Lists
    if(app->variable_item_list_wifi) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewVariableItemListWifi);
        variable_item_list_free(app->variable_item_list_wifi);
    }
    if(app->variable_item_list_file) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewVariableItemListFile);
        variable_item_list_free(app->variable_item_list_file);
    }
    if(app->variable_item_list_request) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewVariableItemListRequest);
        variable_item_list_free(app->variable_item_list_request);
    }

    // Remove and free Text Input views
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputSSID);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputPassword);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputFileType);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputFileRename);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputHeaders);
    view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewTextInputPayload);
    if(app->text_input_path) uart_text_input_free(app->text_input_path);
    if(app->text_input_ssid) uart_text_input_free(app->text_input_ssid);
    if(app->text_input_password) uart_text_input_free(app->text_input_password);
    if(app->text_input_file_type) uart_text_input_free(app->text_input_file_type);
    if(app->text_input_file_rename) uart_text_input_free(app->text_input_file_rename);
    if(app->text_input_headers) uart_text_input_free(app->text_input_headers);
    if(app->text_input_payload) uart_text_input_free(app->text_input_payload);

    // Remove and free Widgets
    if(app->widget_about) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewAbout);
        widget_free(app->widget_about);
    }
    if(app->widget_file_read) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewFileRead);
        widget_free(app->widget_file_read);
    }
    if(app->widget_file_delete) {
        view_dispatcher_remove_view(app->view_dispatcher, WebCrawlerViewFileDelete);
        widget_free(app->widget_file_delete);
    }

    // Free the ViewDispatcher and close GUI
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    // Free the application structure
    if(app) {
        free(app);
    }
}

WebCrawlerApp* app_instance = NULL;
char* http_method_names[] = {"GET", "POST", "PUT", "DELETE", "DOWNLOAD"};
