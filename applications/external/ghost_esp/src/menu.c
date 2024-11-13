#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include "uart_utils.h"
#include "settings_storage.h"
#include "settings_def.h"
#include "confirmation_view.h"

typedef struct {
    const char* label; // Display label in menu
    const char* command; // UART command to send
    const char* capture_prefix; // Prefix for capture files (NULL if none)
    const char* file_ext; // File extension for captures (NULL if none)
    const char* folder; // Folder for captures (NULL if none)
    bool needs_input; // Whether command requires text input
    const char* input_text; // Text to show in input box (NULL if none)
    bool needs_confirmation; // Whether command needs confirmation
    const char* confirm_header; // Confirmation dialog header
    const char* confirm_text; // Confirmation dialog text
    const char* details_header; // Header for details view
    const char* details_text; // Detailed description/info text
} MenuCommand;

typedef struct {
    AppState* state;
    const MenuCommand* command;
} MenuCommandContext;

// Function declarations
static void show_menu(
    AppState* state,
    const MenuCommand* commands,
    size_t command_count,
    const char* header,
    Submenu* menu,
    uint8_t view_id);
static void show_command_details(AppState* state, const MenuCommand* command);
static bool menu_input_handler(InputEvent* event, void* context);
static void text_input_result_callback(void* context);

// WiFi menu command definitions - grouped by function
// WiFi menu command definitions
static const MenuCommand wifi_commands[] = {
    // Scanning Operations
    {
        .label = "Scan WiFi APs",
        .command = "scanap\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "WiFi AP Scanner",
        .details_text = "Scans for nearby WiFi\n"
                        "Access Points and shows:\n"
                        "- Network names (SSIDs)\n"
                        "- Signal strength\n"
                        "- Security type\n"
                        "- Channel\n",
    },
    {
        .label = "Scan WiFi Stations",
        .command = "scansta\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Station Scanner",
        .details_text = "Scans for client devices\n"
                        "connected to networks:\n"
                        "- MAC addresses\n"
                        "- Connected SSID\n"
                        "- Signal strength\n",
    },
    {
        .label = "Stop Scan",
        .command = "stopscan\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop Scanning",
        .details_text = "Stops any active WiFi\n"
                        "scanning operations\n"
                        "currently running.\n",
    },
    {
        .label = "List APs",
        .command = "list -a\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "List Access Points",
        .details_text = "Shows list of APs found\n"
                        "during last scan with:\n"
                        "- Network details\n"
                        "- Channel info\n"
                        "- Security type\n",

    },
    {
        .label = "List Stations",
        .command = "list -s\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "List Stations",
        .details_text = "Shows list of clients\n"
                        "found during last scan:\n"
                        "- Device MAC address\n"
                        "- Connected network\n"
                        "- Signal strength\n",
    },
    {
        .label = "Select AP",
        .command = "select -a",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = true,
        .input_text = "AP Number",
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Select Access Point",
        .details_text = "Select an AP by number\n"
                        "from the scanned list\n"
                        "for targeting with\n"
                        "other commands.\n",
    },

    // Beacon Spam Operations
    {
        .label = "Beacon Spam (List)",
        .command = "beaconspam -l\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "List Beacon Spam",
        .details_text = "Broadcasts fake APs\n"
                        "using names from a\n"
                        "predefined list.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Beacon Spam (Random)",
        .command = "beaconspam -r\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Random Beacon Spam",
        .details_text = "Broadcasts fake APs\n"
                        "with randomly generated\n"
                        "network names.\n"
                        "Range: ~50-100m\n",

    },
    {
        .label = "Beacon Spam (Rickroll)",
        .command = "beaconspam -rr\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Rickroll Beacons",
        .details_text = "Broadcasts fake APs\n"
                        "with names from Rick\n"
                        "Astley lyrics.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Custom Beacon Spam",
        .command = "beaconspam",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = true,
        .input_text = "SSID Name",
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Custom Beacons",
        .details_text = "Broadcasts fake APs\n"
                        "using your custom\n"
                        "network name.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Stop Spam",
        .command = "stopspam\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop Beacon Spam",
        .details_text = "Stops any active\n"
                        "beacon spam attacks\n"
                        "currently running.\n",
    },

    // Attack Operations
    {
        .label = "Deauth",
        .command = "attack -d\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Deauth Attack",
        .details_text = "Sends deauth frames to\n"
                        "disconnect clients from\n"
                        "selected network.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Stop Deauth",
        .command = "stopdeauth\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop Deauth",
        .details_text = "Stops any active\n"
                        "deauthentication\n"
                        "attacks running.\n",
    },

    // Capture Operations
    {
        .label = "Sniff Raw Packets",
        .command = "capture -raw\n",
        .capture_prefix = "raw_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Raw Packet Capture",
        .details_text = "Captures all WiFi\n"
                        "packets in range.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Sniff PMKID",
        .command = "capture -eapol\n",
        .capture_prefix = "pmkid_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "PMKID Capture",
        .details_text = "Captures PMKID/EAPOL\n"
                        "frames for cracking.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Sniff Probes",
        .command = "capture -probe\n",
        .capture_prefix = "probes_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Probe Capture",
        .details_text = "Captures probe\n"
                        "requests from clients.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Sniff WPS",
        .command = "capture -wps\n",
        .capture_prefix = "wps_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "WPS Capture",
        .details_text = "Captures WPS data\n"
                        "exchanges & beacons.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Sniff Deauth",
        .command = "capture -deauth\n",
        .capture_prefix = "deauth_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Deauth Capture",
        .details_text = "Records deauth and\n"
                        "disassoc frames.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Sniff Beacons",
        .command = "capture -beacon\n",
        .capture_prefix = "beacon_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Beacon Capture",
        .details_text = "Captures beacon\n"
                        "frames from APs.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },
    {
        .label = "Stop Capture",
        .command = "capture -stop\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop Capture",
        .details_text = "Stops any active\n"
                        "packet captures and\n"
                        "saves the PCAP file.\n",
    },
    {
        .label = "Sniff Pwnagotchi",
        .command = "capture -pwn\n",
        .capture_prefix = "pwn_capture",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Pwnagotchi Capture",
        .details_text = "Captures packets from\n"
                        "Pwnagotchi devices.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50-100m\n",
    },

    // Portal & Network Operations
    {
        .label = "Evil Portal",
        .command = "startportal\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = true,
        .confirm_header = "Evil Portal",
        .confirm_text = "You need to configure\n settings in the WebUI\n for this command.\n\n",
        .details_header = "Evil Portal",
        .details_text = "Starts captive portal\n"
                        "for credential harvest.\n"
                        "Configure in WebUI:\n"
                        "- Portal settings\n"
                        "- Landing page\n"
                        "- Credentials DB\n",
    },
    {
        .label = "Stop Portal",
        .command = "stopportal\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop Portal",
        .details_text = "Stops the running\n"
                        "Evil Portal and saves\n"
                        "captured credentials.\n",
    },
    {
        .label = "Connect To WiFi",
        .command = "connect",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = true,
        .input_text = "SSID,Password",
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "WiFi Connect",
        .details_text = "Connect ESP to WiFi:\n"
                        "Enter SSID & password\n"
                        "separated by comma.\n"
                        "Example: network,pass\n",
    },
    {
        .label = "Cast Random Video",
        .command = "dialconnect\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Video Cast",
        .details_text = "Casts random videos\n"
                        "to nearby Cast/DIAL\n"
                        "enabled devices.\n"
                        "Range: ~50m\n",
    },
    {
        .label = "Printer Power",
        .command = "powerprinter\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = true,
        .confirm_header = "Printer Power",
        .confirm_text = "You need to configure\n settings in the WebUI\n for this command.\n",
        .details_header = "WiFi Printer",
        .details_text = "Control power state\n"
                        "of network printers.\n"
                        "Configure in WebUI:\n"
                        "- Printer IP/Port\n"
                        "- Protocol type\n",
    },
};

// BLE menu command definitions
static const MenuCommand ble_commands[] = {
    {
        .label = "Find the Flippers",
        .command = "blescan -f\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Flipper Scanner",
        .details_text = "Scans for nearby\n"
                        "Flipper Zero devices.\n"
                        "Shows device info:\n"
                        "- Name & Address\n"
                        "- Signal strength\n",
    },
    {
        .label = "AirTag Scanner",
        .command = "blescan -a\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "AirTag Scanner",
        .details_text = "Detects nearby Apple\n"
                        "AirTags and shows:\n"
                        "- Device ID\n"
                        "- Signal strength\n"
                        "- Last seen time\n",
    },
    {
        .label = "Sniff Bluetooth",
        .command = "blescan -r\n",
        .capture_prefix = "btscan",
        .file_ext = "pcap",
        .folder = GHOST_ESP_APP_FOLDER_PCAPS,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "BLE Sniffer",
        .details_text = "Captures Bluetooth LE\n"
                        "packets & adv data.\n"
                        "Saves to PCAP file.\n"
                        "Range: ~50m\n",
    },
    {
        .label = "Stop BLE Scan",
        .command = "blescan -s\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop BLE Scan",
        .details_text = "Stops any active\n"
                        "Bluetooth scanning\n"
                        "operations.\n",
    },
};

// GPS menu command definitions
static const MenuCommand gps_commands[] = {
    {
        .label = "WarDrive Mode",
        .command = "startwd\n",
        .capture_prefix = "wardrive_scan",
        .file_ext = "csv",
        .folder = GHOST_ESP_APP_FOLDER_WARDRIVE,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "WarDriving Mode",
        .details_text = "Maps WiFi networks\n"
                        "with GPS location.\n"
                        "Saves data to CSV:\n"
                        "- Network details\n"
                        "- GPS coordinates\n"
                        "- Signal strength\n",
    },
    {
        .label = "Stop WarDrive",
        .command = "startwd -s\n",
        .capture_prefix = NULL,
        .file_ext = NULL,
        .folder = NULL,
        .needs_input = false,
        .input_text = NULL,
        .needs_confirmation = false,
        .confirm_header = NULL,
        .confirm_text = NULL,
        .details_header = "Stop WarDrive",
        .details_text = "Stops wardriving\n"
                        "mode and saves any\n"
                        "remaining data.\n",
    }};

void send_uart_command(const char* command, void* state) {
    AppState* app_state = (AppState*)state;
    uart_send(app_state->uart_context, (uint8_t*)command, strlen(command));
}

void send_uart_command_with_text(const char* command, char* text, AppState* state) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s %s\n", command, text);
    uart_send(state->uart_context, (uint8_t*)buffer, strlen(buffer));
}

void send_uart_command_with_bytes(
    const char* command,
    const uint8_t* bytes,
    size_t length,
    AppState* state) {
    send_uart_command(command, state);
    uart_send(state->uart_context, bytes, length);
}

static void confirmation_ok_callback(void* context) {
    MenuCommandContext* cmd_ctx = context;
    if(cmd_ctx && cmd_ctx->state && cmd_ctx->command) {
        bool file_opened = false;

        // **Step 1: Open the PCAP File First**
        if(cmd_ctx->command->capture_prefix || cmd_ctx->command->file_ext ||
           cmd_ctx->command->folder) {
            FURI_LOG_I("Capture", "Attempting to open PCAP file before sending capture command.");
            file_opened = uart_receive_data(
                cmd_ctx->state->uart_context,
                cmd_ctx->state->view_dispatcher,
                cmd_ctx->state,
                cmd_ctx->command->capture_prefix ? cmd_ctx->command->capture_prefix : "",
                cmd_ctx->command->file_ext ? cmd_ctx->command->file_ext : "",
                cmd_ctx->command->folder ? cmd_ctx->command->folder : "");

            if(!file_opened) {
                FURI_LOG_E("Capture", "Failed to open PCAP file. Aborting capture command.");
                free(cmd_ctx);
                return;
            }
        }

        // **Step 2: Send the Capture Command After File is Opened**
        send_uart_command(cmd_ctx->command->command, cmd_ctx->state);
        FURI_LOG_I("Capture", "Capture command sent to firmware.");
    }
    free(cmd_ctx);
}

static void confirmation_cancel_callback(void* context) {
    MenuCommandContext* cmd_ctx = context;
    if(cmd_ctx && cmd_ctx->state) {
        switch(cmd_ctx->state->previous_view) {
        case 1:
            show_wifi_menu(cmd_ctx->state);
            break;
        case 2:
            show_ble_menu(cmd_ctx->state);
            break;
        case 3:
            show_gps_menu(cmd_ctx->state);
            break;
        default:
            show_main_menu(cmd_ctx->state);
            break;
        }
    }
    free(cmd_ctx);
}

// Add at top with other declarations:
static void app_info_ok_callback(void* context) {
    AppState* state = context;
    if(!state) return;

    view_dispatcher_switch_to_view(state->view_dispatcher, state->previous_view);
    state->current_view = state->previous_view;
}

static void show_command_details(AppState* state, const MenuCommand* command) {
    if(!command->details_header || !command->details_text) return;

    // Save current view before switching
    state->previous_view = state->current_view;

    // Setup confirmation view to show details
    confirmation_view_set_header(state->confirmation_view, command->details_header);
    confirmation_view_set_text(state->confirmation_view, command->details_text);

    // Set up callbacks for OK/Cancel to return to previous view
    confirmation_view_set_ok_callback(
        state->confirmation_view,
        app_info_ok_callback, // Reuse app info callback since it does the same thing
        state);
    confirmation_view_set_cancel_callback(state->confirmation_view, app_info_ok_callback, state);

    // Switch to confirmation view
    view_dispatcher_switch_to_view(state->view_dispatcher, 7);
    state->current_view = 7;
}

static void error_callback(void* context) {
    AppState* state = (AppState*)context;
    if(!state) return;
    view_dispatcher_switch_to_view(state->view_dispatcher, state->previous_view);
    state->current_view = state->previous_view;
}

static void execute_menu_command(AppState* state, const MenuCommand* command) {
    // Check ESP connection first
    if(!uart_is_esp_connected(state->uart_context)) {
        // Save current view
        state->previous_view = state->current_view;

        // Show error and return
        confirmation_view_set_header(state->confirmation_view, "Connection Error");
        confirmation_view_set_text(
            state->confirmation_view,
            "ESP Not Connected!\nTry Rebooting ESP.\nReflash if issues persist.");
        confirmation_view_set_ok_callback(state->confirmation_view, error_callback, state);
        confirmation_view_set_cancel_callback(state->confirmation_view, error_callback, state);

        view_dispatcher_switch_to_view(state->view_dispatcher, 7);
        state->current_view = 7;
        return; // Important: Return here to prevent further execution
    }

    // For commands needing confirmation
    if(command->needs_confirmation) {
        MenuCommandContext* cmd_ctx = malloc(sizeof(MenuCommandContext));
        cmd_ctx->state = state;
        cmd_ctx->command = command;
        confirmation_view_set_header(state->confirmation_view, command->confirm_header);
        confirmation_view_set_text(state->confirmation_view, command->confirm_text);
        confirmation_view_set_ok_callback(
            state->confirmation_view, confirmation_ok_callback, cmd_ctx);
        confirmation_view_set_cancel_callback(
            state->confirmation_view, confirmation_cancel_callback, cmd_ctx);

        view_dispatcher_switch_to_view(state->view_dispatcher, 7);
        return; // Important: Return here
    }

    // For commands needing input
    if(command->needs_input) {
        state->uart_command = command->command;
        text_input_reset(state->text_input);
        text_input_set_header_text(state->text_input, command->input_text);
        text_input_set_result_callback(
            state->text_input, text_input_result_callback, state, state->input_buffer, 32, true);
        view_dispatcher_switch_to_view(state->view_dispatcher, 6);
        return; // Important: Return here
    }

    // Handle capture commands
    if(command->capture_prefix || command->file_ext || command->folder) {
        // First open the capture file and switch view
        bool file_opened = uart_receive_data(
            state->uart_context,
            state->view_dispatcher,
            state,
            command->capture_prefix ? command->capture_prefix : "",
            command->file_ext ? command->file_ext : "",
            command->folder ? command->folder : "");

        if(!file_opened) {
            FURI_LOG_E("Capture", "Failed to open capture file");
            return;
        }

        // Then send command to start capture only after file is ready
        furi_delay_ms(10); // Small delay to ensure file is ready
        send_uart_command(command->command, state);
        return; // Important: Return here
    }

    // For regular commands:
    // 1. First send the command
    send_uart_command(command->command, state);

    // 2. Then switch to text view
    uart_receive_data(
        state->uart_context,
        state->view_dispatcher,
        state,
        "", // No capture prefix
        "", // No file extension
        ""); // No folder
}
// Menu display functions
void show_wifi_menu(AppState* state) {
    show_menu(
        state, wifi_commands, COUNT_OF(wifi_commands), "WiFi Utilities:", state->wifi_menu, 1);
}

void show_ble_menu(AppState* state) {
    show_menu(state, ble_commands, COUNT_OF(ble_commands), "BLE Utilities:", state->ble_menu, 2);
}

void show_gps_menu(AppState* state) {
    show_menu(state, gps_commands, COUNT_OF(gps_commands), "GPS Utilities:", state->gps_menu, 3);
}

// Menu command handlers
void handle_wifi_menu(AppState* state, uint32_t index) {
    if(index < COUNT_OF(wifi_commands)) {
        state->last_wifi_index = index; // Save the selection
        execute_menu_command(state, &wifi_commands[index]);
    }
}

void handle_ble_menu(AppState* state, uint32_t index) {
    if(index < COUNT_OF(ble_commands)) {
        state->last_ble_index = index; // Save the selection
        execute_menu_command(state, &ble_commands[index]);
    }
}

void handle_gps_menu(AppState* state, uint32_t index) {
    if(index < COUNT_OF(gps_commands)) {
        state->last_gps_index = index; // Save the selection
        execute_menu_command(state, &gps_commands[index]);
    }
}

// Callback for text input results
static void text_input_result_callback(void* context) {
    AppState* input_state = (AppState*)context;
    send_uart_command_with_text(
        input_state->uart_command, input_state->input_buffer, (AppState*)input_state);
    uart_receive_data(
        input_state->uart_context, input_state->view_dispatcher, input_state, "", "", "");
}

void submenu_callback(void* context, uint32_t index) {
    AppState* state = (AppState*)context;
    state->current_index = index; // Track current selection

    switch(state->current_view) {
    case 0: // Main Menu
        switch(index) {
        case 0:
            show_wifi_menu(state);
            break;
        case 1:
            show_ble_menu(state);
            break;
        case 2:
            show_gps_menu(state);
            break;
        case 3: // Settings button
            view_dispatcher_switch_to_view(state->view_dispatcher, 8);
            state->current_view = 8;
            state->previous_view = 8;
            break;
        }
        break;
    case 1:
        handle_wifi_menu(state, index);
        break;
    case 2:
        handle_ble_menu(state, index);
        break;
    case 3:
        handle_gps_menu(state, index);
        break;
    }
}

static void show_menu_help(void* context, uint32_t index) {
    UNUSED(index);
    AppState* state = context;

    // Save current view
    state->previous_view = state->current_view;

    // Define help text with essential actions only
    const char* help_text = "Hold [Ok]\n    Show details for commands\n\n"
                            "[OPTIONAL]\nBack in output view\nstops all operations.\n\n\n";

    // Set header and help text in the confirmation view
    confirmation_view_set_header(state->confirmation_view, "Quick Help");
    confirmation_view_set_text(state->confirmation_view, help_text);

    // Set callbacks for user actions
    confirmation_view_set_ok_callback(state->confirmation_view, app_info_ok_callback, state);
    confirmation_view_set_cancel_callback(state->confirmation_view, app_info_ok_callback, state);

    // Switch to confirmation view to display help
    view_dispatcher_switch_to_view(state->view_dispatcher, 7);
    state->current_view = 7;
}

bool back_event_callback(void* context) {
    AppState* state = (AppState*)context;
    if(!state) return false;

    uint32_t current_view = state->current_view;

    // Allow confirmation view to handle its own back button
    if(current_view == 7) {
        return false;
    }

    // Handle text box view (view 5)
    if(current_view == 5) {
        FURI_LOG_D("Ghost ESP", "Handling text box view exit");

        // Cleanup text buffer
        if(state->textBoxBuffer) {
            free(state->textBoxBuffer);
            state->textBoxBuffer = malloc(1);
            if(state->textBoxBuffer) {
                state->textBoxBuffer[0] = '\0';
            }
            state->buffer_length = 0;
        }

        // Send stop commands if enabled in settings
        if(state->settings.stop_on_back_index) {
            FURI_LOG_D("Ghost ESP", "Stopping active operations");

            // First stop any packet captures to ensure proper file saving
            send_uart_command("capture -stop\n", state);

            // Give more time for PCAP stop command to process
            furi_delay_ms(200);

            // Wait for any pending PCAP data
            if(state->uart_context->pcap) {
                // Wait for pcap buffer to empty
                for(uint8_t i = 0; i < 10; i++) { // Try up to 10 times
                    if(state->uart_context->pcap_buf_len > 0) {
                        furi_delay_ms(50);
                    } else {
                        break;
                    }
                }

                // Now safe to reset PCAP state
                state->uart_context->pcap = false;
                furi_stream_buffer_reset(state->uart_context->pcap_stream);
            }

            // Stop operations in a logical order
            const char* stop_commands[] = {
                "stopscan\n",
                "stopspam\n",
                "stopdeauth\n",
                "stopportal\n",
                "blescan -s\n",
                "stop\n"};

            for(size_t i = 0; i < COUNT_OF(stop_commands); i++) {
                send_uart_command(stop_commands[i], state);
                furi_delay_ms(50);
            }
        }

        // Clean up files using the safe cleanup
        if(state->uart_context && state->uart_context->storageContext) {
            uart_storage_safe_cleanup(state->uart_context->storageContext);
            FURI_LOG_D("Ghost ESP", "Performed safe storage cleanup");
        }

        // Return to previous menu with selection restored
        switch(state->previous_view) {
        case 1:
            show_wifi_menu(state);
            submenu_set_selected_item(state->wifi_menu, state->last_wifi_index);
            break;
        case 2:
            show_ble_menu(state);
            submenu_set_selected_item(state->ble_menu, state->last_ble_index);
            break;
        case 3:
            show_gps_menu(state);
            submenu_set_selected_item(state->gps_menu, state->last_gps_index);
            break;
        case 8:
            view_dispatcher_switch_to_view(state->view_dispatcher, 8);
            break;
        default:
            show_main_menu(state);
            break;
        }
        state->current_view = state->previous_view;
    }
    // Handle settings menu (view 8)
    else if(current_view == 8) {
        show_main_menu(state);
        state->current_view = 0;
    }
    // Handle settings submenu (view 4)
    else if(current_view == 4) {
        view_dispatcher_switch_to_view(state->view_dispatcher, 8);
        state->current_view = 8;
    }
    // Handle submenu views (1-3)
    else if(current_view >= 1 && current_view <= 3) {
        show_main_menu(state);
        state->current_view = 0;
    }
    // Handle text input view (view 6)
    else if(current_view == 6) {
        // Return to previous menu with selection restored
        switch(state->previous_view) {
        case 1:
            show_wifi_menu(state);
            submenu_set_selected_item(state->wifi_menu, state->last_wifi_index);
            break;
        case 2:
            show_ble_menu(state);
            submenu_set_selected_item(state->ble_menu, state->last_ble_index);
            break;
        case 3:
            show_gps_menu(state);
            submenu_set_selected_item(state->gps_menu, state->last_gps_index);
            break;
        default:
            show_main_menu(state);
            break;
        }
        state->current_view = state->previous_view;
    }
    // Handle main menu (view 0)
    else if(current_view == 0) {
        view_dispatcher_stop(state->view_dispatcher);
    }

    return true;
}

static void show_menu(
    AppState* state,
    const MenuCommand* commands,
    size_t command_count,
    const char* header,
    Submenu* menu,
    uint8_t view_id) {
    submenu_reset(menu);
    submenu_set_header(menu, header);

    for(size_t i = 0; i < command_count; i++) {
        submenu_add_item(menu, commands[i].label, i, submenu_callback, state);
    }

    // Set up view with input handler
    View* menu_view = submenu_get_view(menu);
    view_set_context(menu_view, state);
    view_set_input_callback(menu_view, menu_input_handler);

    // Restore last selection based on menu type
    uint32_t last_index = 0;
    switch(view_id) {
    case 1:
        last_index = state->last_wifi_index;
        break;
    case 2:
        last_index = state->last_ble_index;
        break;
    case 3:
        last_index = state->last_gps_index;
        break;
    }
    if(last_index < command_count) {
        submenu_set_selected_item(menu, last_index);
    }

    view_dispatcher_switch_to_view(state->view_dispatcher, view_id);
    state->current_view = view_id;
    state->previous_view = view_id;
}

void show_main_menu(AppState* state) {
    main_menu_reset(state->main_menu);
    main_menu_set_header(state->main_menu, ""); // Empty header since we're drawing our own
    main_menu_add_item(state->main_menu, "WiFi", 0, submenu_callback, state);
    main_menu_add_item(state->main_menu, "BLE", 1, submenu_callback, state);
    main_menu_add_item(state->main_menu, "GPS", 2, submenu_callback, state); // GPS restored
    main_menu_add_item(state->main_menu, " SET", 3, submenu_callback, state);

    // Set up help callback
    main_menu_set_help_callback(state->main_menu, show_menu_help, state);

    view_dispatcher_switch_to_view(state->view_dispatcher, 0);
    state->current_view = 0;
}

static bool menu_input_handler(InputEvent* event, void* context) {
    AppState* state = (AppState*)context;
    bool consumed = false;

    if(!state || !event) return false;

    const MenuCommand* commands = NULL;
    size_t commands_count = 0;
    Submenu* current_menu = NULL;

    // Determine current menu context
    switch(state->current_view) {
    case 1:
        current_menu = state->wifi_menu;
        commands = wifi_commands;
        commands_count = COUNT_OF(wifi_commands);
        break;
    case 2:
        current_menu = state->ble_menu;
        commands = ble_commands;
        commands_count = COUNT_OF(ble_commands);
        break;
    case 3:
        current_menu = state->gps_menu;
        commands = gps_commands;
        commands_count = COUNT_OF(gps_commands);
        break;
    default:
        return false;
    }

    if(!current_menu || !commands) return false;

    uint32_t current_index = submenu_get_selected_item(current_menu);

    switch(event->type) {
    case InputTypeShort:
        switch(event->key) {
        case InputKeyUp:
            if(current_index > 0) {
                submenu_set_selected_item(current_menu, current_index - 1);
            }
            consumed = true;
            break;

        case InputKeyDown:
            if(current_index < commands_count - 1) {
                submenu_set_selected_item(current_menu, current_index + 1);
            }
            consumed = true;
            break;

        case InputKeyOk:
            if(current_index < commands_count) {
                state->current_index = current_index;
                execute_menu_command(state, &commands[current_index]);
                consumed = true;
            }
            break;

        case InputKeyBack:
            show_main_menu(state);
            state->current_view = 0;
            consumed = true;
            break;

        case InputKeyRight:
        case InputKeyLeft:
        case InputKeyMAX:
            break;
        }
        break;

    case InputTypeLong:
        switch(event->key) {
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyRight:
        case InputKeyLeft:
        case InputKeyBack:
        case InputKeyMAX:
            break;

        case InputKeyOk:
            if(current_index < commands_count) {
                const MenuCommand* command = &commands[current_index];
                if(command->details_header && command->details_text) {
                    show_command_details(state, command);
                    consumed = true;
                }
            }
            break;
        }
        break;

    case InputTypeRepeat:
        switch(event->key) {
        case InputKeyUp:
            if(current_index > 0) {
                submenu_set_selected_item(current_menu, current_index - 1);
            }
            consumed = true;
            break;

        case InputKeyDown:
            if(current_index < commands_count - 1) {
                submenu_set_selected_item(current_menu, current_index + 1);
            }
            consumed = true;
            break;

        case InputKeyRight:
        case InputKeyLeft:
        case InputKeyOk:
        case InputKeyBack:
        case InputKeyMAX:
            break;
        }
        break;

    case InputTypePress:
    case InputTypeRelease:
    case InputTypeMAX:
        break;
    }

    return consumed;
}
