#pragma once
#include <furi_hal.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_box.h>
#include <gui/modules/submenu.h>
#include "gui_modules/mainmenu.h"
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include "app_state.h"

// Function declarations
void send_uart_command(const char* command, void* state); // Changed from AppState* to void*
void send_uart_command_with_text(const char* command, char* text, AppState* state);
void send_uart_command_with_bytes(
    const char* command,
    const uint8_t* bytes,
    size_t length,
    AppState* state);

bool back_event_callback(void* context);
void submenu_callback(void* context, uint32_t index);
void handle_wifi_commands(AppState* state, uint32_t index, const char** wifi_commands);
void show_main_menu(AppState* state);
void handle_wifi_menu(AppState* state, uint32_t index);
void handle_ble_menu(AppState* state, uint32_t index);
void handle_gps_menu(AppState* state, uint32_t index);

void show_wifi_menu(AppState* state);
void show_ble_menu(AppState* state);
void show_gps_menu(AppState* state);
