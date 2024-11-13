#ifndef FLIP_WEATHER_CALLBACK_H
#define FLIP_WEATHER_CALLBACK_H

#include "flip_weather.h"
#include <parse/flip_weather_parse.h>
#include <flip_storage/flip_weather_storage.h>

extern bool weather_request_success;
extern bool sent_weather_request;
extern bool got_weather_data;

void flip_weather_popup_callback(void* context);
void flip_weather_request_error(Canvas* canvas);
void flip_weather_handle_gps_draw(Canvas* canvas, bool show_gps_data);
void flip_weather_view_draw_callback_weather(Canvas* canvas, void* model);
void flip_weather_view_draw_callback_gps(Canvas* canvas, void* model);
void callback_submenu_choices(void* context, uint32_t index);
void text_updated_ssid(void* context);
void text_updated_password(void* context);
uint32_t callback_to_submenu(void* context);
void settings_item_selected(void* context, uint32_t index);

/**
 * @brief Navigation callback for exiting the application
 * @param context The context - unused
 * @return next view id (VIEW_NONE to exit the app)
 */
uint32_t callback_exit_app(void* context);
uint32_t callback_to_wifi_settings(void* context);
#endif
