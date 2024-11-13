#pragma once
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include "gui_modules/mainmenu.h"
#include "settings_def.h"
#include "app_types.h"
#include "settings_ui_types.h"

typedef struct {
    bool enabled; // Master switch for filtering
    bool show_ble_status;
    bool show_wifi_status;
    bool show_flipper_devices;
    bool show_wifi_networks;
    bool strip_ansi_codes;
    bool add_prefixes; // Whether to add [BLE], [WIFI] etc prefixes
} FilterConfig;

struct AppState {
    // Views
    ViewDispatcher* view_dispatcher;
    MainMenu* main_menu;
    Submenu* wifi_menu;
    Submenu* ble_menu;
    Submenu* gps_menu;
    VariableItemList* settings_menu;
    TextBox* text_box;
    TextInput* text_input;
    ConfirmationView* confirmation_view;
    FuriMutex* buffer_mutex;
    // UART Context
    UartContext* uart_context;
    FilterConfig* filter_config;

    // Settings
    Settings settings;
    SettingsUIContext settings_ui_context;
    Submenu* settings_actions_menu;

    // State
    uint32_t current_index;
    uint8_t current_view;
    uint8_t previous_view;
    uint32_t last_wifi_index;
    uint32_t last_ble_index;
    uint32_t last_gps_index;
    char* input_buffer;
    const char* uart_command;
    char* textBoxBuffer;
    size_t buffer_length;
    size_t buffer_capacity;
    size_t buffer_size;
};
