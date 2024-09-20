// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2024 KBEmbedded

#include <furi.h>
#include <gui/modules/submenu.h>
#include <storage/storage.h>
#include <src/include/fgp_app.h>
#include <src/scenes/include/fgp_scene.h>

// gblink protocol support
#include <protocols/printer/include/printer_proto.h>
#include <protocols/printer/include/printer_receive.h>

#include <src/include/file_handling.h>
#include <src/include/png.h>
#include <src/include/tile_tools.h>

/* XXX: TODO turn this in to an enum */
#define DATA     0x80000000
#define PRINT    0x40000000
#define COMPLETE 0x20000000

#define WIDTH  160L
#define HEIGHT 144L

struct recv_model {
    int count;
    int converted;
    int errors;
};

struct recv_ctx {
    View* view;
    struct recv_model* model;
    struct fgp_app* fgp;
    FuriTimer* timer;
    ViewDispatcher* view_dispatcher;

    // Printer handling
    void* printer_handle;

    struct gb_image* volatile_image;
    struct gb_image* image;
    int packet_cnt;

    // PNG handling
    void* png_handle;

    // File operations
    void* file_handle;
};

static void fgp_receive_view_timer(void* context) {
    struct recv_ctx* ctx = context;

    with_view_model(ctx->view, struct recv_model * model, { UNUSED(model); }, true);
}

static void printer_callback(void* context, struct gb_image* image, enum cb_reason reason) {
    struct recv_ctx* ctx = context;

    FURI_LOG_D("printer", "printer_callback reason %d", (int)reason);

    switch(reason) {
    case reason_data:
        ctx->packet_cnt++;
        view_dispatcher_send_custom_event(ctx->view_dispatcher, DATA);
        break;
    case reason_print:
        /* TODO: XXX: 
		 * Interleave prints and processing here.
		 * For the first photo, its safe to mark this as "printed" immediately
		 * after copying the buffer. This lets the next print start quickly, e.g.
		 * for doing a print all from Photo! or other prints that may take multiple
		 * prints like panoramics.
		 * Once the next photo comes in, we need to check if the save process is
		 * still running, if not, then repeat above. If so, then I don't know yet.
		 * Need to put this somewhere, somehow, maybe have a custom event re-call
		 * this?
		 * Set up a return.
		 * Hmm.
		 * This logic might need to move to the event handler, not here. In that
		 * case, we probably need to double buffer. So if the first print is not
		 * yet done, the second one can be copied here, and the event can, do something
		 * with that and hold off on marking it printed.
		 */
        ctx->volatile_image = image;
        view_dispatcher_send_custom_event(ctx->view_dispatcher, PRINT);
        break;
    case reason_complete:
        view_dispatcher_send_custom_event(ctx->view_dispatcher, COMPLETE);
        break;
    }
}

static bool fgp_receive_view_event(uint32_t event, void* context) {
    struct recv_ctx* ctx = context;
    struct gb_image* image = ctx->image;
    bool consumed = false;
    bool error = false;
    FuriString* fs_tmp;
    uint8_t px_y = 0;
    uint8_t px_x = 0;

    if(event == DATA) consumed = true;

    if(event == PRINT) {
        fs_tmp = furi_string_alloc();
        /* Copy the buffer from the printer data here, and then tell the
		 * printer it can continue receiving.
		 */
        /* XXX: TODO: NOTE: I believe this is safe as I don't think this
		 * event callback handle will be called again until this completes.
		 * I need to verify which thread calls this and how the queue is
		 * managed.
		 */
        /* This copies everything except data */
        memcpy(image, ctx->volatile_image, sizeof(struct gb_image));

        /* Prep some image information */
        px_x = 160; // TODO: Photo! transfer will be less than this
        px_y = image->data_sz / 40; // 40 is bytes per line, 160 px / 4 (px/byte)

        /* Now copy the image data, but as scanlines rather than tiles */
        /* Tiles are 8x8 px */
        tile_to_scanline(image->data, ctx->volatile_image->data, px_x / 8, px_y / 8);

        printer_receive_print_complete(ctx->printer_handle);
        with_view_model(ctx->view, struct recv_model * model, { model->count++; }, false);

        /* Save binary version always */
        error |= !fgp_storage_open(ctx->file_handle, ".bin");
        error |= !fgp_storage_write(ctx->file_handle, image->data, image->data_sz);
        error |= !fgp_storage_close(ctx->file_handle);

        if(ctx->fgp->add_header) {
            error |= !fgp_storage_open(ctx->file_handle, "-hdr.bin");
            error |= !fgp_storage_write(ctx->file_handle, "GB-BIN01", 8);
            error |= !fgp_storage_write(ctx->file_handle, image->data, image->data_sz);
            error |= !fgp_storage_close(ctx->file_handle);
        }

        /* Save PNG */
        png_reset(ctx->png_handle, px_x, px_y);
        png_populate(ctx->png_handle, image->data);
        png_palette_set(ctx->png_handle, palette_rgb16_get(ctx->fgp->palette_idx));
        furi_string_printf(fs_tmp, "-%s.png", palette_shortname_get(ctx->fgp->palette_idx));
        error |= !fgp_storage_open(ctx->file_handle, furi_string_get_cstr(fs_tmp));
        error |= !fgp_storage_write(
            ctx->file_handle, png_buf_get(ctx->png_handle), png_len_get(ctx->png_handle));
        error |= !fgp_storage_close(ctx->file_handle);

        fgp_storage_next_count(ctx->file_handle);

        furi_string_free(fs_tmp);

        if(!error) {
            with_view_model(ctx->view, struct recv_model * model, { model->converted++; }, false);
        } else {
            with_view_model(ctx->view, struct recv_model * model, { model->errors++; }, false);
        }

        consumed = true;
    }

    return consumed;
}

static void fgp_receive_view_enter(void* context) {
    struct recv_ctx* ctx = context;

    view_allocate_model(ctx->view, ViewModelTypeLockFree, sizeof(struct recv_model));

    /* XXX: TODO: Startup of this takes a long time. Presumably, it is all
	 * of the storage allocation, directory creation, finding the latest
	 * file number, etc. Because this runs at a higher priority than the
	 * timer callback, setting up the draw timer first has no impact on
	 * the issue.
	 * I believe draw calls are also in the same context as this thread,
	 * so I don't think there is a good way to issue a draw callback here.
	 * Need to figure out a better way to handle this setup.
	 */
    ctx->image = printer_image_buffer_alloc();
    ctx->printer_handle = ctx->fgp->printer_handle;

    ctx->file_handle = fgp_storage_alloc("GCIM_", ".bin");

    printer_callback_context_set(ctx->printer_handle, ctx);
    printer_callback_set(ctx->printer_handle, printer_callback);
    printer_receive_start(ctx->printer_handle);

    ctx->png_handle = png_alloc(160, 144);

    ctx->timer = furi_timer_alloc(fgp_receive_view_timer, FuriTimerTypePeriodic, ctx);
    furi_timer_start(ctx->timer, furi_ms_to_ticks(200));
}

static void fgp_receive_view_exit(void* context) {
    struct recv_ctx* ctx = context;

    fgp_storage_free(ctx->file_handle);
    furi_timer_free(ctx->timer);

    png_free(ctx->png_handle);

    printer_stop(ctx->printer_handle);

    printer_image_buffer_free(ctx->image);
    view_free_model(ctx->view);
}

static bool fgp_receive_view_input(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    bool ret = false;

    return ret;
}

static void fgp_receive_view_draw(Canvas* canvas, void* view_model) {
    struct recv_model* model = view_model;
    char string[26];
    snprintf(string, sizeof(string), "Received: %d", model->count);
    canvas_draw_str(canvas, 18, 13, string);
    snprintf(string, sizeof(string), "Converted to PNG: %d", model->converted);
    canvas_draw_str(canvas, 18, 21, string);
    snprintf(string, sizeof(string), "Conversion errors: %d", model->errors);
    canvas_draw_str(canvas, 18, 29, string);
}

View* fgp_receive_view_get_view(void* context) {
    struct recv_ctx* ctx = context;
    return ctx->view;
}

void* fgp_receive_view_alloc(struct fgp_app* fgp) {
    struct recv_ctx* ctx = malloc(sizeof(struct recv_ctx));

    ctx->view_dispatcher = fgp->view_dispatcher;
    ctx->fgp = fgp;

    ctx->view = view_alloc();
    view_set_context(ctx->view, ctx);

    view_set_input_callback(ctx->view, fgp_receive_view_input);
    view_set_draw_callback(ctx->view, fgp_receive_view_draw);
    view_set_enter_callback(ctx->view, fgp_receive_view_enter);
    view_set_exit_callback(ctx->view, fgp_receive_view_exit);
    view_set_custom_callback(ctx->view, fgp_receive_view_event);

    return ctx;
}

void fgp_receive_view_free(void* recv_ctx) {
    struct recv_ctx* ctx = recv_ctx;

    view_free(ctx->view);
    free(ctx);
}
