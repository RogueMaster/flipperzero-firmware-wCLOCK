#include "callbacks.h"
#include "app_state.h"
#include "menu.h"
#include "settings_def.h"
#include "settings_ui.h"
#include "uart_utils.h"

void on_rgb_mode_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.rgb_mode_index = index;
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_RGB_MODE[index]);
    char command[32];
    snprintf(command, sizeof(command), "setsetting -i 1 -v %d\n", index + 1);
    send_uart_command(command, app);
}

void on_channelswitchdelay_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.channel_hop_delay_index = index;
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_CHANNEL_HOP[index]);
    char command[32];
    snprintf(command, sizeof(command), "setsetting -i 2 -v %d\n", index + 1);
    send_uart_command(command, app);
}

void on_togglechannelhopping_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.enable_channel_hopping_index = index;
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_BOOL[index]);
    char command[32];
    snprintf(command, sizeof(command), "setsetting -i 3 -v %d\n", index + 1);
    send_uart_command(command, app);
}

void on_ble_mac_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.enable_random_ble_mac_index = index;
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_BOOL[index]);
    char command[32];
    snprintf(command, sizeof(command), "setsetting -i 4 -v %d\n", index + 1);
    send_uart_command(command, app);
}

void on_stop_on_back_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.stop_on_back_index = index;
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_BOOL[index]);
}

void on_reboot_esp_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_ACTION[index]);
    send_uart_command("handle_reboot\n", app);
}

void logs_clear_confirmed_callback(void* context) {
    FURI_LOG_D("ClearLogs", "Confirmed callback started, context: %p", context);

    SettingsConfirmContext* ctx = context;
    if(!ctx) {
        FURI_LOG_E("ClearLogs", "Null context");
        return;
    }

    if(!ctx->state) {
        FURI_LOG_E("ClearLogs", "Null state in context");
        free(ctx);
        return;
    }

    AppState* app_state = ctx->state;
    uint32_t prev_view = app_state->previous_view;

    FURI_LOG_D("ClearLogs", "Previous view: %lu", prev_view);
    clear_log_files(ctx->state);

    // Reset callbacks
    FURI_LOG_D("ClearLogs", "Resetting callbacks");
    confirmation_view_set_ok_callback(app_state->confirmation_view, NULL, NULL);
    confirmation_view_set_cancel_callback(app_state->confirmation_view, NULL, NULL);

    // Free context first
    free(ctx);

    // Switch view last and update current_view
    FURI_LOG_D("ClearLogs", "Switching to view: %lu", prev_view);
    view_dispatcher_switch_to_view(app_state->view_dispatcher, prev_view);
    app_state->current_view = prev_view; // Add this line
}

void logs_clear_cancelled_callback(void* context) {
    FURI_LOG_D("ClearLogs", "Cancel callback started, context: %p", context);

    SettingsConfirmContext* ctx = context;
    if(!ctx) {
        FURI_LOG_E("ClearLogs", "Null context");
        return;
    }

    if(!ctx->state) {
        FURI_LOG_E("ClearLogs", "Null state in context");
        free(ctx);
        return;
    }

    AppState* app_state = ctx->state;
    uint32_t prev_view = app_state->previous_view;

    FURI_LOG_D("ClearLogs", "Previous view: %lu", prev_view);

    // Reset callbacks before freeing context
    FURI_LOG_D("ClearLogs", "Resetting callbacks");
    confirmation_view_set_ok_callback(app_state->confirmation_view, NULL, NULL);
    confirmation_view_set_cancel_callback(app_state->confirmation_view, NULL, NULL);

    // Free context
    free(ctx);

    // Switch view last and update current_view
    FURI_LOG_D("ClearLogs", "Switching to view: %lu", prev_view);
    view_dispatcher_switch_to_view(app_state->view_dispatcher, prev_view);
    app_state->current_view = prev_view; // Add this line
}

void on_clear_logs_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_ACTION[index]);

    if(index == 0) {
        show_confirmation_dialog_ex(
            app,
            "Clear Logs",
            "Are you sure you want to clear the logs?\nThis action cannot be undone.",
            logs_clear_confirmed_callback,
            logs_clear_cancelled_callback);
    }
}

void on_clear_nvs_changed(VariableItem* item) {
    AppState* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, SETTING_VALUE_NAMES_ACTION[index]);

    if(index == 0) {
        show_confirmation_dialog_ex( // Changed to _ex version
            app,
            "Clear NVS",
            "Are you sure you want to clear NVS?\nThis will reset all ESP settings.",
            nvs_clear_confirmed_callback,
            nvs_clear_cancelled_callback);
    }
}

void nvs_clear_confirmed_callback(void* context) {
    SettingsConfirmContext* ctx = context;
    if(ctx && ctx->state) {
        AppState* app_state = ctx->state;
        uint32_t prev_view = app_state->previous_view;

        send_uart_command("handle_clearnvs\n", ctx->state);

        confirmation_view_set_ok_callback(app_state->confirmation_view, NULL, NULL);
        confirmation_view_set_cancel_callback(app_state->confirmation_view, NULL, NULL);

        free(ctx);

        view_dispatcher_switch_to_view(app_state->view_dispatcher, prev_view);
        app_state->current_view = prev_view; // Add this line
    }
}

void nvs_clear_cancelled_callback(void* context) {
    SettingsConfirmContext* ctx = context;
    if(ctx && ctx->state) {
        AppState* app_state = ctx->state;
        uint32_t prev_view = app_state->previous_view;

        confirmation_view_set_ok_callback(app_state->confirmation_view, NULL, NULL);
        confirmation_view_set_cancel_callback(app_state->confirmation_view, NULL, NULL);

        free(ctx);

        view_dispatcher_switch_to_view(app_state->view_dispatcher, prev_view);
        app_state->current_view = prev_view; // Add this line
    }
}
void show_app_info(void* context) {
    SettingsUIContext* settings_context = (SettingsUIContext*)context;
    AppState* app = (AppState*)settings_context->context;

    FURI_LOG_D("AppInfo", "Show app info called, context: %p", app);

    const char* info_text = "";

    if(app && app->confirmation_view) {
        // Create a new context for the confirmation dialog
        SettingsConfirmContext* confirm_ctx = malloc(sizeof(SettingsConfirmContext));
        if(!confirm_ctx) {
            FURI_LOG_E("AppInfo", "Failed to allocate confirmation context");
            return;
        }
        confirm_ctx->state = app;

        // Save current view before switching
        app->previous_view = app->current_view;
        FURI_LOG_D("AppInfo", "Saved previous view: %d", app->previous_view);

        confirmation_view_set_header(app->confirmation_view, "App Info");
        confirmation_view_set_text(app->confirmation_view, info_text);

        // Set up callbacks with proper context
        confirmation_view_set_ok_callback(
            app->confirmation_view, app_info_ok_callback, confirm_ctx);
        confirmation_view_set_cancel_callback(
            app->confirmation_view, app_info_cancel_callback, confirm_ctx);

        // Switch to confirmation view
        FURI_LOG_D("AppInfo", "Switching to confirmation view");
        view_dispatcher_switch_to_view(app->view_dispatcher, 7); // 7 is confirmation view
        app->current_view = 7;
    } else {
        FURI_LOG_E("AppInfo", "Invalid app state or confirmation view");
    }
}

// Update callback functions to use proper context
void app_info_ok_callback(void* context) {
    SettingsConfirmContext* ctx = (SettingsConfirmContext*)context;
    if(!ctx || !ctx->state) {
        FURI_LOG_E("AppInfo", "Invalid callback context");
        return;
    }

    AppState* app_state = ctx->state;
    uint32_t prev_view = app_state->previous_view;

    FURI_LOG_D("AppInfo", "OK callback, returning to view: %lu", prev_view);

    // Reset callbacks
    confirmation_view_set_ok_callback(app_state->confirmation_view, NULL, NULL);
    confirmation_view_set_cancel_callback(app_state->confirmation_view, NULL, NULL);

    // Free the context
    free(ctx);

    // Return to previous view
    view_dispatcher_switch_to_view(app_state->view_dispatcher, prev_view);
    app_state->current_view = prev_view;
}

void app_info_cancel_callback(void* context) {
    // Use same callback as OK since both just return to previous view
    app_info_ok_callback(context);
}
