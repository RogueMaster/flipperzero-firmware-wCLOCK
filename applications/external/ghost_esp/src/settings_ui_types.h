#pragma once
#include "settings_def.h"
#include "settings_ui.h"

// Full structure definition
struct SettingsUIContext {
    Settings* settings;
    SendUartCommandCallback send_uart_command;
    SwitchToViewCallback switch_to_view;
    ShowConfirmationViewCallback show_confirmation_view;
    void* context;
};
