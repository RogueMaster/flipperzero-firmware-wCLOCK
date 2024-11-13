#ifndef FLIP_STORE_E_H
#define FLIP_STORE_E_H
#include <text_input/uart_text_input.h>
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
#include <flip_store_icons.h>
#define TAG "FlipStore"

// define the list of categories
extern char* categories[];
extern char* firmwares[];

// Define the submenu items for our FlipStore application
typedef enum {
    FlipStoreSubmenuIndexMain, // Click to start downloading the selected app
    FlipStoreSubmenuIndexAbout,
    FlipStoreSubmenuIndexSettings,
    //
    FlipStoreSubmenuIndexOptions, // Click to view the options
    //
    FlipStoreSubmenuIndexAppList,
    FlipStoreSubmenuIndexFirmwares,
    //
    FlipStoreSubmenuIndexAppListBluetooth,
    FlipStoreSubmenuIndexAppListGames,
    FlipStoreSubmenuIndexAppListGPIO,
    FlipStoreSubmenuIndexAppListInfrared,
    FlipStoreSubmenuIndexAppListiButton,
    FlipStoreSubmenuIndexAppListMedia,
    FlipStoreSubmenuIndexAppListNFC,
    FlipStoreSubmenuIndexAppListRFID,
    FlipStoreSubmenuIndexAppListSubGHz,
    FlipStoreSubmenuIndexAppListTools,
    FlipStoreSubmenuIndexAppListUSB,
    //
    FlipStoreSubmenuIndexStartFirmwares,
    //
    FlipStoreSubmenuIndexStartAppList = 100,
} FlipStoreSubmenuIndex;

// Define a single view for our FlipStore application
typedef enum {
    FlipStoreViewMain, // The main screen
    //
    FlipStoreViewSubmenu, // The submenu
    FlipStoreViewSubmenuOptions, // The submenu options
    //
    FlipStoreViewAbout, // The about screen
    FlipStoreViewSettings, // The settings screen
    FlipStoreViewTextInputSSID, // The text input screen for SSID
    FlipStoreViewTextInputPass, // The text input screen for password
    //
    FlipStoreViewPopup, // The popup screen
    //
    FlipStoreViewAppList, // The app list screen
    FlipStoreViewFirmwares, // The firmwares screen
    //
    FlipStoreViewAppInfo, // The app info screen (widget) of the selected app
    FlipStoreViewAppDownload, // The app download screen (widget) of the selected app
    FlipStoreViewAppDelete, // The app delete screen (DialogEx) of the selected app
    //
    FlipStoreViewAppListBluetooth, // the app list screen for Bluetooth
    FlipStoreViewAppListGames, // the app list screen for Games
    FlipStoreViewAppListGPIO, // the app list screen for GPIO
    FlipStoreViewAppListInfrared, // the app list screen for Infrared
    FlipStoreViewAppListiButton, // the app list screen for iButton
    FlipStoreViewAppListMedia, // the app list screen for Media
    FlipStoreViewAppListNFC, // the app list screen for NFC
    FlipStoreViewAppListRFID, // the app list screen for RFID
    FlipStoreViewAppListSubGHz, // the app list screen for Sub-GHz
    FlipStoreViewAppListTools, // the app list screen for Tools
    FlipStoreViewAppListUSB, // the app list screen for USB
} FlipStoreView;

// Each screen will have its own view
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    View* view_main; // The main screen for downloading apps
    View* view_app_info; // The app info screen (view) of the selected app
    Submenu* submenu_main; // The submenu (main)
    //
    Submenu* submenu_options; // The submenu (options)
    Submenu* submenu_app_list; // The submenu (app list) for the selected category
    Submenu* submenu_firmwares; // The submenu (firmwares)
    //
    Submenu* submenu_app_list_bluetooth; // The submenu (app list) for Bluetooth
    Submenu* submenu_app_list_games; // The submenu (app list) for Games
    Submenu* submenu_app_list_gpio; // The submenu (app list) for GPIO
    Submenu* submenu_app_list_infrared; // The submenu (app list) for Infrared
    Submenu* submenu_app_list_ibutton; // The submenu (app list) for iButton
    Submenu* submenu_app_list_media; // The submenu (app list) for Media
    Submenu* submenu_app_list_nfc; // The submenu (app list) for NFC
    Submenu* submenu_app_list_rfid; // The submenu (app list) for RFID
    Submenu* submenu_app_list_subghz; // The submenu (app list) for Sub-GHz
    Submenu* submenu_app_list_tools; // The submenu (app list) for Tools
    Submenu* submenu_app_list_usb; // The submenu (app list) for USB
    //
    Widget* widget; // The widget
    Popup* popup; // The popup
    DialogEx* dialog_delete; // The dialog for deleting an app
    VariableItemList* variable_item_list; // The variable item list (settngs)
    VariableItem* variable_item_ssid; // The variable item
    VariableItem* variable_item_pass; // The variable item
    UART_TextInput* uart_text_input_ssid; // The text input
    UART_TextInput* uart_text_input_pass; // The text input

    char* uart_text_input_buffer_ssid; // Buffer for the text input
    char* uart_text_input_temp_buffer_ssid; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_ssid; // Size of the text input buffer

    char* uart_text_input_buffer_pass; // Buffer for the text input
    char* uart_text_input_temp_buffer_pass; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_pass; // Size of the text input buffer
} FlipStoreApp;

void flip_store_app_free(FlipStoreApp* app);

#endif // FLIP_STORE_E_H
