// web_crawler_e.h
#ifndef WEB_CRAWLER_E
#define WEB_CRAWLER_E

#include <easy_flipper/easy_flipper.h>
#include <flipper_http/flipper_http.h>
#include <jsmn/jsmn.h>
#include <text_input/uart_text_input.h>
#include "web_crawler_icons.h"
#include <storage/storage.h>

#define TAG "Web Crawler"
extern char* http_method_names[];

// Define the submenu items for our WebCrawler application
typedef enum {
    WebCrawlerSubmenuIndexRun, // click to go to Run the GET request
    WebCrawlerSubmenuIndexAbout, // click to go to About screen
    WebCrawlerSubmenuIndexConfig, // click to go to Config submenu (Wifi, File)
    WebCrawlerSubmenuIndexRequest, // click to go to Request submenu (Set URL, HTTP Method, Headers)
    WebCrawlerSubmenuIndexWifi, // click to go to Wifi submenu (SSID, Password)
    WebCrawlerSubmenuIndexFile, // click to go to file submenu (Read, File Type, Rename, Delete)
} WebCrawlerSubmenuIndex;

typedef enum {
    WebCrawlerViewRun, // Run the GET request
    WebCrawlerViewAbout, // About screen
    WebCrawlerViewSubmenuConfig, // Submenu Config view for App (Wifi, File)
    WebCrawlerViewVariableItemListRequest, // Submenu for URL (Set URL, HTTP Method, Headers)
    WebCrawlerViewVariableItemListWifi, // Wifi Configuration screen (Submenu for SSID, Password)
    WebCrawlerViewVariableItemListFile, // Submenu for File (Read, File Type, Rename, Delete)
    WebCrawlerViewMain, // Main view for App
    WebCrawlerViewSubmenuMain, // Submenu Main view for App (Run, About, Config)
    WebCrawlerViewTextInput, // Text input for Path
    WebCrawlerViewTextInputSSID, // Text input for SSID
    WebCrawlerViewTextInputPassword, // Text input for Password
    WebCrawlerViewFileRead, // File Read
    WebCrawlerViewTextInputFileType, // Text input for File Type
    WebCrawlerViewTextInputFileRename, // Text input for File Rename
    WebCrawlerViewTextInputHeaders, // Text input for Headers
    WebCrawlerViewTextInputPayload, // Text input for Payload
    WebCrawlerViewFileDelete, // File Delete
} WebCrawlerViewIndex;

// Define the application structure
typedef struct {
    ViewDispatcher* view_dispatcher;
    View* view_main;
    View* view_run;
    Submenu* submenu_main;
    Submenu* submenu_config;
    Widget* widget_about;

    UART_TextInput* text_input_path;
    UART_TextInput* text_input_ssid;
    UART_TextInput* text_input_password;
    UART_TextInput* text_input_file_type;
    UART_TextInput* text_input_file_rename;
    //
    UART_TextInput* text_input_headers;
    UART_TextInput* text_input_payload;

    Widget* widget_file_read;
    Widget* widget_file_delete;

    VariableItemList* variable_item_list_wifi;
    VariableItemList* variable_item_list_file;
    VariableItemList* variable_item_list_request;

    VariableItem* path_item;
    VariableItem* ssid_item;
    VariableItem* password_item;
    VariableItem* file_type_item;
    VariableItem* file_rename_item;
    VariableItem* file_read_item;
    VariableItem* file_delete_item;
    //
    VariableItem* http_method_item;
    VariableItem* headers_item;
    VariableItem* payload_item;

    char* path;
    char* ssid;
    char* password;
    char* file_type;
    char* file_rename;
    char* http_method;
    char* headers;
    char* payload;

    char* temp_buffer_path;
    uint32_t temp_buffer_size_path;

    char* temp_buffer_ssid;
    uint32_t temp_buffer_size_ssid;

    char* temp_buffer_password;
    uint32_t temp_buffer_size_password;

    char* temp_buffer_file_type;
    uint32_t temp_buffer_size_file_type;

    char* temp_buffer_file_rename;
    uint32_t temp_buffer_size_file_rename;

    char* temp_buffer_http_method;
    uint32_t temp_buffer_size_http_method;

    char* temp_buffer_headers;
    uint32_t temp_buffer_size_headers;

    char* temp_buffer_payload;
    uint32_t temp_buffer_size_payload;
} WebCrawlerApp;

void free_buffers(WebCrawlerApp* app);

void free_resources(WebCrawlerApp* app);

void free_all(WebCrawlerApp* app, char* reason);

/**
 * @brief      Function to free the resources used by WebCrawlerApp.
 * @param      app  The WebCrawlerApp object to free.
 */
void web_crawler_app_free(WebCrawlerApp* app);

extern WebCrawlerApp* app_instance;

#endif // WEB_CRAWLER_E
