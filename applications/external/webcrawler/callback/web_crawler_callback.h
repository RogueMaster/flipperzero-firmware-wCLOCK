#ifndef WEB_CRAWLER_CALLBACK_H
#define WEB_CRAWLER_CALLBACK_H
#include "web_crawler.h"
#include <flip_storage/web_crawler_storage.h>

extern bool sent_http_request;
extern bool get_success;
extern bool already_success;

void web_crawler_draw_error(Canvas* canvas);

void web_crawler_http_method_change(VariableItem* item);

void web_crawler_view_draw_callback(Canvas* canvas, void* context);

/**
 * @brief      Navigation callback to handle exiting from other views to the submenu.
 * @param      context   The context - WebCrawlerApp object.
 * @return     WebCrawlerViewSubmenu
 */
uint32_t web_crawler_back_to_configure_callback(void* context);

/**
 * @brief      Navigation callback to handle returning to the Wifi Settings screen.
 * @param      context   The context - WebCrawlerApp object.
 * @return     WebCrawlerViewSubmenu
 */
uint32_t web_crawler_back_to_main_callback(void* context);
uint32_t web_crawler_back_to_file_callback(void* context);

uint32_t web_crawler_back_to_wifi_callback(void* context);

uint32_t web_crawler_back_to_request_callback(void* context);

/**
 * @brief      Navigation callback to handle exiting the app from the main submenu.
 * @param      context   The context - unused
 * @return     VIEW_NONE to exit the app
 */
uint32_t web_crawler_exit_app_callback(void* context);

/**
 * @brief      Handle submenu item selection.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The WebCrawlerSubmenuIndex item that was clicked.
 */
void web_crawler_submenu_callback(void* context, uint32_t index);

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_wifi_enter_callback(void* context, uint32_t index);

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_file_enter_callback(void* context, uint32_t index);

/**
 * @brief      Configuration enter callback to handle different items.
 * @param      context   The context - WebCrawlerApp object.
 * @param      index     The index of the item that was clicked.
 */
void web_crawler_request_enter_callback(void* context, uint32_t index);

/**
 * @brief      Callback for when the user finishes entering the URL.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_path_updated(void* context);

/**
 * @brief      Callback for when the user finishes entering the headers
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_headers_updated(void* context);

/**
 * @brief      Callback for when the user finishes entering the payload.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_payload_updated(void* context);

/**
 * @brief      Callback for when the user finishes entering the SSID.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_ssid_updated(void* context);

/**
 * @brief      Callback for when the user finishes entering the Password.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_password_update(void* context);

/**
 * @brief      Callback for when the user finishes entering the File Type.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_file_type_update(void* context);

/**
 * @brief      Callback for when the user finishes entering the File Rename.
 * @param      context   The context - WebCrawlerApp object.
 */
void web_crawler_set_file_rename_update(void* context);

/**
 * @brief      Handler for Path configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_path_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for headers configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_headers_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for payload configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_payload_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for SSID configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_ssid_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for Password configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_password_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for File Type configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_type_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for File Rename configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_rename_clicked(void* context, uint32_t index);

/**
 * @brief      Handler for File Delete configuration item click.
 * @param      context  The context - WebCrawlerApp object.
 * @param      index    The index of the item that was clicked.
 */
void web_crawler_setting_item_file_delete_clicked(void* context, uint32_t index);

void web_crawler_setting_item_file_read_clicked(void* context, uint32_t index);
#endif
