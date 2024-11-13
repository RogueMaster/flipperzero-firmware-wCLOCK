#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_power.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_box.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <expansion/expansion.h>

#include "menu.h"
#include "uart_utils.h"
#include "settings_storage.h"
#include "settings_def.h"
#include "settings_ui.h"
#include "app_state.h"
#include "callbacks.h"
#include "confirmation_view.h"
#include "utils.h"

// Include the header where settings_custom_event_callback is declared
#include "settings_ui.h"

#define UART_INIT_STACK_SIZE 2048
static int32_t init_uart_task(void* context) {
    AppState* state = context;

    // Add some delay to let system stabilize
    furi_delay_ms(50);

    state->uart_context = uart_init(state);
    if(state->uart_context) {
        FURI_LOG_I("Ghost_ESP", "UART initialized successfully");
    } else {
        FURI_LOG_E("Ghost_ESP", "UART initialization failed");
    }
    return 0;
}

int32_t ghost_esp_app(void* p) {
    UNUSED(p);

    // Disable expansion protocol to avoid UART interference
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    // Power initialization
    uint8_t attempts = 0;
    bool otg_was_enabled = furi_hal_power_is_otg_enabled();
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    furi_delay_ms(200); // Longer delay for power stabilization

    // Set up bare minimum UI state
    AppState* state = malloc(sizeof(AppState));
    if(!state) return -1;
    memset(state, 0, sizeof(AppState)); // Zero all memory first

    // Initialize menu selection indices
    state->last_wifi_index = 0;
    state->last_ble_index = 0;
    state->last_gps_index = 0;
    state->current_index = 0;
    state->current_view = 0;
    state->previous_view = 0;

    // Initialize essential text buffers with minimal size
    state->textBoxBuffer = malloc(1);
    if(state->textBoxBuffer) {
        state->textBoxBuffer[0] = '\0';
    }
    state->buffer_length = 0;
    state->input_buffer = malloc(32);
    if(state->input_buffer) {
        memset(state->input_buffer, 0, 32);
    }

    // Initialize UI components - core components first
    state->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(state->view_dispatcher);
    state->main_menu = main_menu_alloc();
    if(!state->view_dispatcher || !state->main_menu) {
        // Clean up and exit if core components fail
        if(state->view_dispatcher) view_dispatcher_free(state->view_dispatcher);
        if(state->main_menu) main_menu_free(state->main_menu);
        free(state->textBoxBuffer);
        free(state->input_buffer);
        free(state);
        return -1;
    }

    // Allocate remaining UI components
    state->wifi_menu = submenu_alloc();
    state->ble_menu = submenu_alloc();
    state->gps_menu = submenu_alloc();
    state->text_box = text_box_alloc();
    state->settings_menu = variable_item_list_alloc();
    state->text_input = text_input_alloc();
    state->confirmation_view = confirmation_view_alloc();
    state->settings_actions_menu = submenu_alloc();

    // Set headers - only for successfully allocated components
    if(state->main_menu) main_menu_set_header(state->main_menu, "Select a Utility");
    if(state->wifi_menu) submenu_set_header(state->wifi_menu, "Select a Wifi Utility");
    if(state->ble_menu) submenu_set_header(state->ble_menu, "Select a Bluetooth Utility");
    if(state->gps_menu) submenu_set_header(state->gps_menu, "Select a GPS Utility");
    if(state->text_input) text_input_set_header_text(state->text_input, "Enter Your Text");
    if(state->settings_actions_menu) submenu_set_header(state->settings_actions_menu, "Settings");

    // Initialize settings and configuration early
    settings_storage_init();
    if(settings_storage_load(&state->settings, GHOST_ESP_APP_SETTINGS_FILE) != SETTINGS_OK) {
        memset(&state->settings, 0, sizeof(Settings));
        state->settings.stop_on_back_index = 1;
        settings_storage_save(&state->settings, GHOST_ESP_APP_SETTINGS_FILE);
    }

    // Initialize filter config
    state->filter_config = malloc(sizeof(FilterConfig));
    if(state->filter_config) {
        state->filter_config->enabled = state->settings.enable_filtering_index;
        state->filter_config->show_ble_status = true;
        state->filter_config->show_wifi_status = true;
        state->filter_config->show_flipper_devices = true;
        state->filter_config->show_wifi_networks = true;
        state->filter_config->strip_ansi_codes = true;
        state->filter_config->add_prefixes = true;
    }

    // Set up settings UI context
    state->settings_ui_context.settings = &state->settings;
    state->settings_ui_context.send_uart_command = send_uart_command;
    state->settings_ui_context.switch_to_view = NULL;
    state->settings_ui_context.show_confirmation_view = show_confirmation_view_wrapper;
    state->settings_ui_context.context = state;

    // Initialize settings menu
    settings_setup_gui(state->settings_menu, &state->settings_ui_context);

    // Start UART init in background thread
    FuriThread* uart_init_thread =
        furi_thread_alloc_ex("UartInit", UART_INIT_STACK_SIZE, init_uart_task, state);

    // Add views to dispatcher - check each component before adding
    if(state->view_dispatcher) {
        if(state->main_menu)
            view_dispatcher_add_view(
                state->view_dispatcher, 0, main_menu_get_view(state->main_menu));
        if(state->wifi_menu)
            view_dispatcher_add_view(
                state->view_dispatcher, 1, submenu_get_view(state->wifi_menu));
        if(state->ble_menu)
            view_dispatcher_add_view(state->view_dispatcher, 2, submenu_get_view(state->ble_menu));
        if(state->gps_menu)
            view_dispatcher_add_view(state->view_dispatcher, 3, submenu_get_view(state->gps_menu));
        if(state->settings_menu)
            view_dispatcher_add_view(
                state->view_dispatcher, 4, variable_item_list_get_view(state->settings_menu));
        if(state->text_box)
            view_dispatcher_add_view(
                state->view_dispatcher, 5, text_box_get_view(state->text_box));
        if(state->text_input)
            view_dispatcher_add_view(
                state->view_dispatcher, 6, text_input_get_view(state->text_input));
        if(state->confirmation_view)
            view_dispatcher_add_view(
                state->view_dispatcher, 7, confirmation_view_get_view(state->confirmation_view));
        if(state->settings_actions_menu)
            view_dispatcher_add_view(
                state->view_dispatcher, 8, submenu_get_view(state->settings_actions_menu));

        view_dispatcher_set_custom_event_callback(
            state->view_dispatcher, settings_custom_event_callback);
    }

    // Show main menu immediately
    show_main_menu(state);

    // Initialize UART in background
    state->uart_context = uart_init(state);

    // Set up and run GUI
    Gui* gui = furi_record_open("gui");
    if(gui && state->view_dispatcher) {
        view_dispatcher_attach_to_gui(state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
        view_dispatcher_set_navigation_event_callback(state->view_dispatcher, back_event_callback);
        view_dispatcher_set_event_callback_context(state->view_dispatcher, state);
        view_dispatcher_run(state->view_dispatcher);
    }
    furi_record_close("gui");

    // Wait for UART initialization to complete
    furi_thread_join(uart_init_thread);
    furi_thread_free(uart_init_thread);

    // Start cleanup - first remove views
    if(state->view_dispatcher) {
        for(size_t i = 0; i <= 8; i++) {
            view_dispatcher_remove_view(state->view_dispatcher, i);
        }
    }

    // Clear callbacks before cleanup
    if(state->confirmation_view) {
        confirmation_view_set_ok_callback(state->confirmation_view, NULL, NULL);
        confirmation_view_set_cancel_callback(state->confirmation_view, NULL, NULL);
    }

    // Clean up UART first
    if(state->uart_context) {
        uart_free(state->uart_context);
        state->uart_context = NULL;
    }

    // Cleanup UI components in reverse order
    if(state->confirmation_view) {
        confirmation_view_free(state->confirmation_view);
        state->confirmation_view = NULL;
    }
    if(state->text_input) {
        text_input_free(state->text_input);
        state->text_input = NULL;
    }
    if(state->text_box) {
        text_box_free(state->text_box);
        state->text_box = NULL;
    }
    if(state->settings_actions_menu) {
        submenu_free(state->settings_actions_menu);
        state->settings_actions_menu = NULL;
    }
    if(state->settings_menu) {
        variable_item_list_free(state->settings_menu);
        state->settings_menu = NULL;
    }
    if(state->wifi_menu) {
        submenu_free(state->wifi_menu);
        state->wifi_menu = NULL;
    }
    if(state->ble_menu) {
        submenu_free(state->ble_menu);
        state->ble_menu = NULL;
    }
    if(state->gps_menu) {
        submenu_free(state->gps_menu);
        state->gps_menu = NULL;
    }
    if(state->main_menu) {
        main_menu_free(state->main_menu);
        state->main_menu = NULL;
    }

    // Free view dispatcher last after all views are removed
    if(state->view_dispatcher) {
        view_dispatcher_free(state->view_dispatcher);
        state->view_dispatcher = NULL;
    }

    // Cleanup buffers
    if(state->input_buffer) {
        free(state->input_buffer);
        state->input_buffer = NULL;
    }
    if(state->textBoxBuffer) {
        free(state->textBoxBuffer);
        state->textBoxBuffer = NULL;
    }
    // Add filter config cleanup
    if(state->filter_config) {
        free(state->filter_config);
        state->filter_config = NULL;
    }

    // Final state cleanup
    free(state);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    // Power cleanup
    if(furi_hal_power_is_otg_enabled() && !otg_was_enabled) {
        furi_hal_power_disable_otg();
    }

    return 0;
}
