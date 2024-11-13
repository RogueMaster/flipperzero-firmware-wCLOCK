#ifndef FLIP_TRADER_CALLBACK_H
#define FLIP_TRADER_CALLBACK_H

#define MAX_TOKENS 32 // Adjust based on expected JSON size (25)
#include <flip_trader.h>
#include <flip_storage/flip_trader_storage.h>

// hold the price of the asset
extern char asset_price[64];
extern bool sent_get_request;
extern bool get_request_success;
extern bool request_processed;

void flip_trader_request_error(Canvas* canvas);
bool send_price_request();
void process_asset_price();

// Callback for drawing the main screen
void flip_trader_view_draw_callback(Canvas* canvas, void* model);
// Input callback for the view (async input handling)
bool flip_trader_view_input_callback(InputEvent* event, void* context);
void callback_submenu_choices(void* context, uint32_t index);
void text_updated_ssid(void* context);

void text_updated_password(void* context);

uint32_t callback_to_submenu(void* context);

uint32_t callback_to_wifi_settings(void* context);
uint32_t callback_to_assets_submenu(void* context);
void settings_item_selected(void* context, uint32_t index);
#endif // FLIP_TRADER_CALLBACK_H
