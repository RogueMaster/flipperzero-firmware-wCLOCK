/**
 * @file widget_element_i.h
 * GUI: internal Widget Element API
 */

#pragma once

#include <furi.h>
#include <gui/view.h>
#include <input/input.h>
#include <gui/modules/widget_elements/widget_element.h>

#include "../../utils/unicode_utils.h"
#include "../elements.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WidgetElement WidgetElement;
typedef struct Widget Widget;

struct WidgetElement {
    // generic draw and input callbacks
    void (*draw)(Canvas* canvas, WidgetElement* element);
    bool (*input)(InputEvent* event, WidgetElement* element);

    // free callback
    void (*free)(WidgetElement* element);

    // generic model holder
    void* model;
    FuriMutex* model_mutex;

    // pointer to widget that hold our element
    Widget* parent;
};

/** Create multi string element */
WidgetElement* widget_element_string_multiline_create(
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Create string element */
WidgetElement* widget_element_string_create(
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    const char* text);

/** Create text box element */
WidgetElement* widget_element_text_box_create(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    Align horizontal,
    Align vertical,
    const char* text,
    bool strip_to_dots);

/** Update text box element */
void widget_element_text_box_set_text(WidgetElement* gui_string, const char* text);

/** Create button element */
WidgetElement* widget_element_button_create(
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context);

WidgetElement* widget_element_text_scroll_create(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    const char* text);

#ifdef __cplusplus
}
#endif
