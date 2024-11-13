#include "enc_reader.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_power.h>

#include <gui/gui.h>
#include <gui/elements.h>

#include <input/input.h>

#include <notification/notification.h>

static void enc_reader_app_input_callback(InputEvent* input_event, void* context) {
    furi_assert(context);

    FuriMessageQueue* event_queue = context;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void enc_reader_app_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    EncApp* app = context;

    char abs_coord[] = "00000000 ";
    char org_coord[] = "00000000 ";
    char rel_coord[] = "00000000 ";

    static const uint8_t header_height = 12;
    static const uint8_t vertical_gap = 2;
    static const uint8_t vertical_offset = 16;

    snprintf(abs_coord, strlen(abs_coord), "%8d", (int)app->coordinates.abs);
    snprintf(org_coord, strlen(org_coord), "%8d", (int)app->coordinates.org);
    snprintf(rel_coord, strlen(rel_coord), "%8d", (int)app->coordinates.rel);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    elements_multiline_text_aligned(
        canvas,
        4,
        vertical_gap,
        AlignLeft,
        AlignTop,
        app->Vbus_state ? "Bus:    enable" : "Bus:   disable");

    elements_frame(
        canvas, 0, header_height, canvas_width(canvas), canvas_height(canvas) - header_height);

    elements_multiline_text_aligned(
        canvas, 4, header_height + vertical_gap, AlignLeft, AlignTop, "Abs:");
    elements_multiline_text_aligned(
        canvas, 4, header_height + vertical_gap + vertical_offset, AlignLeft, AlignTop, "Org:");
    elements_multiline_text_aligned(
        canvas, 4, header_height + vertical_gap + vertical_offset * 2, AlignLeft, AlignTop, "Rel:");

    canvas_set_font(canvas, FontBigNumbers);
    elements_multiline_text_aligned(
        canvas, 30, header_height + vertical_gap, AlignLeft, AlignTop, (const char*)abs_coord);
    elements_multiline_text_aligned(
        canvas,
        30,
        header_height + vertical_gap + vertical_offset,
        AlignLeft,
        AlignTop,
        (const char*)org_coord);
    elements_multiline_text_aligned(
        canvas,
        30,
        header_height + vertical_gap + vertical_offset * 2,
        AlignLeft,
        AlignTop,
        (const char*)rel_coord);
}

static void enc_reader_app_interrupt_callback(void* context) {
    furi_assert(context);
    EncApp* app = context;

    if(furi_hal_gpio_read(app->input_pin.b))
        app->coordinates.abs++;
    else
        app->coordinates.abs--;
}

EncApp* enc_reader_app_alloc() {
    EncApp* app = malloc(sizeof(EncApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->input_pin.a = &gpio_ext_pa4;
    app->input_pin.b = &gpio_ext_pa7;

    furi_hal_gpio_init(app->input_pin.a, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(app->input_pin.b, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

    // Attach interrupt to pin A
    furi_hal_gpio_add_int_callback(app->input_pin.a, enc_reader_app_interrupt_callback, app);
    furi_hal_gpio_enable_int_callback(app->input_pin.a);

    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    view_port_input_callback_set(app->view_port, enc_reader_app_input_callback, app->event_queue);
    view_port_draw_callback_set(app->view_port, enc_reader_app_draw_callback, app);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void enc_reader_app_free(EncApp* app) {
    furi_assert(app);

    furi_record_close(RECORD_NOTIFICATION);

    furi_hal_gpio_disable_int_callback(app->input_pin.a);
    furi_hal_gpio_remove_int_callback(app->input_pin.a);

    furi_message_queue_free(app->event_queue);
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);
}

int32_t enc_reader_app(void* p) {
    UNUSED(p);
    EncApp* app = enc_reader_app_alloc();

    InputEvent event;

    bool running = true;

    while(running) {
        app->coordinates.rel = app->coordinates.abs - app->coordinates.org;

        if(furi_message_queue_get(app->event_queue, &event, 200) == FuriStatusOk) {
            if(event.key == InputKeyBack) {
                if(event.type == InputTypeLong) running = false;
            } else if(event.key == InputKeyOk) {
                if(event.type == InputTypeShort) {
                    app->coordinates.org = app->coordinates.abs;
                    notification_message(app->notifications, &button_led_sequence);
                }
            } else if(event.key == InputKeyDown) {
                if(event.type == InputTypeShort) {
                    app->coordinates.org = 0;
                    app->coordinates.abs = 0;
                    notification_message(app->notifications, &button_led_sequence);
                }
            } else if(event.key == InputKeyUp) {
                if(event.type == InputTypeShort) {
                    app->Vbus_state = !app->Vbus_state;

                    if(app->Vbus_state) {
                        furi_hal_power_enable_otg();
                        notification_message(app->notifications, &vOn_led_sequence);
                    } else {
                        furi_hal_power_disable_otg();
                        notification_message(app->notifications, &vOff_led_sequence);
                    }
                }
            }
        }
    }
    furi_hal_power_disable_otg();
    enc_reader_app_free(app);
    return 0;
}
