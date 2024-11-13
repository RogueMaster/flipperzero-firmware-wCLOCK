#ifndef FLIP_STORE_CALLBACK_H
#define FLIP_STORE_CALLBACK_H
#include <flip_store.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <apps/flip_store_apps.h>
#include <flip_storage/flip_store_storage.h>

#define MAX_LINE_LENGTH 30

extern bool flip_store_app_does_exist;

// Callback for drawing the main screen
void flip_store_view_draw_callback_main(Canvas* canvas, void* model);

// Function to draw the description on the canvas with word wrapping
void draw_description(Canvas* canvas, const char* user_message, int x, int y);

void flip_store_view_draw_callback_app_list(Canvas* canvas, void* model);

bool flip_store_input_callback(InputEvent* event, void* context);

void flip_store_text_updated_ssid(void* context);

void flip_store_text_updated_pass(void* context);

uint32_t callback_to_submenu(void* context);

uint32_t callback_to_submenu_options(void* context);

uint32_t callback_to_app_list(void* context);

void settings_item_selected(void* context, uint32_t index);

void dialog_callback(DialogExResult result, void* context);

void popup_callback(void* context);

uint32_t callback_exit_app(void* context);
void callback_submenu_choices(void* context, uint32_t index);

#endif // FLIP_STORE_CALLBACK_H
