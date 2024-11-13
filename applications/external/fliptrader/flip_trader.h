#ifndef FLIP_TRADE_E_H
#define FLIP_TRADE_E_H

#include <flipper_http/flipper_http.h>
#include <easy_flipper/easy_flipper.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <dialogs/dialogs.h>
#include <jsmn/jsmn.h>

#define TAG "FlipTrader"

// Define the submenu items for our FlipTrader application
typedef enum {
    // FlipTraderSubmenuIndexMain,     // Click to run get the info of the selected pair
    FlipTradeSubmenuIndexAssets, // Click to view the assets screen (ETHUSD, BTCUSD, etc.)
    FlipTraderSubmenuIndexAbout, // Click to view the about screen
    FlipTraderSubmenuIndexSettings, // Click to view the WiFi settings screen
        //
    FlipTraderSubmenuIndexAssetStartIndex, // Start of the submenu items for the assets
} FlipTraderSubmenuIndex;

// Define a single view for our FlipTrader application
typedef enum {
    FlipTraderViewMain, // The screen that displays the info of the selected pair
    FlipTraderViewMainSubmenu, // The main submenu of the FlipTrader app
    FlipTraderViewAbout, // The about screen
    FlipTraderViewWiFiSettings, // The WiFi settings screen
    FlipTraderViewTextInputSSID, // The text input screen for the SSID
    FlipTraderViewTextInputPassword, // The text input screen for the password
    //
    FlipTraderViewAssetsSubmenu, // The submenu for the assets
} FlipTraderView;

// Each screen will have its own view
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    View* view_main; // The main screen that displays "Hello, World!"
    Submenu* submenu_main; // The submenu
    Submenu* submenu_assets; // The submenu for the assets
    Widget* widget; // The widget
    VariableItemList* variable_item_list_wifi; // The variable item list (settngs)
    VariableItem* variable_item_ssid; // The variable item for the SSID
    VariableItem* variable_item_password; // The variable item for the password
    UART_TextInput* uart_text_input_ssid; // The text input for the SSID
    UART_TextInput* uart_text_input_password; // The text input for the password

    char* uart_text_input_buffer_ssid; // Buffer for the text input (SSID)
    char* uart_text_input_temp_buffer_ssid; // Temporary buffer for the text input (SSID)
    uint32_t uart_text_input_buffer_size_ssid; // Size of the text input buffer (SSID)

    char* uart_text_input_buffer_password; // Buffer for the text input (password)
    char* uart_text_input_temp_buffer_password; // Temporary buffer for the text input (password)
    uint32_t uart_text_input_buffer_size_password; // Size of the text input buffer (password)

} FlipTraderApp;

#define ASSET_COUNT 42
extern char** asset_names;

// index
extern uint32_t asset_index;

// Function to free the resources used by FlipTraderApp
void flip_trader_app_free(FlipTraderApp* app);
char** asset_names_alloc();
void asset_names_free(char** names);

#endif // FLIP_TRADE_E_H
