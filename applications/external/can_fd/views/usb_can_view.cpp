/** 
 * @ingroup APP
  @defgroup VIEW
  @brief Application specific VIEW part of APP MVC model : It displays applications informations while running (Transmitted and received bytes on USB and CAN link). The other views (menus) are standard and implemented in furi gui layer.
  @{
  @file usb_can_view.cpp
  @brief VIEW part of APP.
 */

#include "../usb_can_app_i.hpp"
#include <furi_hal.h>
#include <gui/elements.h>
#include "gui/view.h"

/** Information get from @ref MODEL component (via getters in @ref usb_can_bridge.cpp) */
typedef struct {
    uint32_t tx_cnt;
    uint32_t rx_cnt;
    uint8_t vcp_port;
    bool tx_active;
    bool rx_active;
} UsbCanModel;

/** @brief This function displays bytes transmitted and received when application is in operating mode (user selected an item from the menu). It is registered in @ref usb_can_view_alloc().
 * @details This draw callback is application specific and uses lower level features than those used in other scenes (other than @ref SCENE-USB-CAN). Other draw callbacks are standard (item list, error message, popup) and supplied by furi layer. This function will:
 * -# print Virtual COM Port used by formatting @ref UsbCanModel::vcp_port via @ref snprintf() and calling @ref canvas_draw_str_aligned()
 * -# print transmitted bytes by formatting @ref UsbCanModel::tx_cnt via @ref snprintf() (in KiB if transmitted bytes > 1000000) and calling @ref canvas_draw_str_aligned()
 * -# do the same thing for recived bytes (@ref UsbCanModel::rx_cnt)
*/
static void usb_can_draw_callback(Canvas* canvas, void* _model) {
    UsbCanModel* model = (UsbCanModel*)_model;
    char temp_str[18];

    canvas_set_font(canvas, FontSecondary);
    snprintf(temp_str, 18, "COM PORT:%u", model->vcp_port);
    canvas_draw_str_aligned(canvas, 126, 8, AlignRight, AlignBottom, temp_str);

    if(model->tx_cnt < 100000000) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 24, AlignRight, AlignBottom, "B.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->tx_cnt);
        canvas_draw_str_aligned(canvas, 116, 24, AlignRight, AlignBottom, temp_str);
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 24, AlignRight, AlignBottom, "KiB.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->tx_cnt / 1024);
        canvas_draw_str_aligned(canvas, 111, 24, AlignRight, AlignBottom, temp_str);
    }

    if(model->rx_cnt < 100000000) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 41, AlignRight, AlignBottom, "B.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->rx_cnt);
        canvas_draw_str_aligned(canvas, 116, 41, AlignRight, AlignBottom, temp_str);
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 41, AlignRight, AlignBottom, "KiB.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->rx_cnt / 1024);
        canvas_draw_str_aligned(canvas, 111, 41, AlignRight, AlignBottom, temp_str);
    }

    /*if(model->tx_active)
        canvas_draw_icon(canvas, 48, 14, &I_ArrowUpFilled_14x15);
    else
        canvas_draw_icon(canvas, 48, 14, &I_ArrowUpEmpty_14x15);

    if(model->rx_active)
        canvas_draw_icon_ex(canvas, 48, 34, &I_ArrowUpFilled_14x15, IconRotation180);
    else
        canvas_draw_icon_ex(canvas, 48, 34, &I_ArrowUpEmpty_14x15, IconRotation180);*/
}

/** @brief This callback registered in @ref usb_can_view_alloc() and trigged by furi gui component is used to go to @ref SCENE-CONFIG if left arrow key is pressed via @ref usb_can_view_update_state().
 * @details This callback registered in @ref usb_can_view_alloc() is called by furi gui component when a key is pressed. 
 * If it is left arrow it will request to go to @ref SCENE-CONFIG by trigging @ref usb_can_scene_usb_can_callback registered through @ref usb_can_view_update_state(). */
static bool usb_can_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    usbCanView* usb_can_view = (usbCanView*)context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            consumed = true;
            furi_assert(usb_can_view->callback);
            usb_can_view->callback(UsbCanCustomEventConfig, usb_can_view->context);
        }
    }

    return consumed;
}

/** @brief This function allocate view @ref usbCanView::view element and register  @ref usb_can_draw_callback and @ref usb_can_input_callback callbacks to furi gui component.
 * @details This function is called on application start when memory is allocated through @ref usb_can_app_alloc(). It perform following actions:
 * -# allocate memory space for @ref usbCanView object via @ref malloc
 * -# initialize @ref usbCanView:view pointer allocating dedicated memory space via @ref malloc
 * -# allocate memory model (updated via @ref MODEL getters) holding data displayed to the user on flipper screen( @ref UsbCanModel)
 * -# set pointer to be passed to usbCanView:view object callbacks through @ref view_set_context(). Here this is the @ref usbCanView object created holding all needed references to operate the @ref VIEW component.
 * -# register draw callback to furi (Flipper Universal Registry Implementation) gui stack through @ref view_set_draw_callback
 * -# register user input callback to furi gui stack via @ref view_set_input_callback (when a flliper key is pressed)
 * @return a pointer to an allocated and initialized @ref usbCanView object.
*/
usbCanView* usb_can_view_alloc() {
    usbCanView* usb_can_view = (usbCanView*)malloc(sizeof(usbCanView));

    usb_can_view->view = view_alloc();
    view_allocate_model(usb_can_view->view, ViewModelTypeLocking, sizeof(UsbCanModel));
    view_set_context(usb_can_view->view, usb_can_view);
    view_set_draw_callback(usb_can_view->view, usb_can_draw_callback);
    view_set_input_callback(usb_can_view->view, usb_can_input_callback);

    return usb_can_view;
}

/** @brief This function free memory allocated for @ref VIEW component functionning in @ref usb_can_view_alloc.  
 * @details This function is called on application start when memory is allocated through @ref usb_can_app_alloc().
*/
void usb_can_view_free(usbCanView* usb_can_view) {
    furi_assert(usb_can_view);
    view_free(usb_can_view->view);
    free(usb_can_view);
}

/** @brief get @ref View object reference holded by @ref usbCanView object (which correspond to data used by @ref VIEW component) passed in parameter.*/
View* usb_can_get_view(usbCanView* usb_can_view) {
    furi_assert(usb_can_view);
    return usb_can_view->view;
}

/** @brief register callback to @ref VIEW component (see @ref usbCanView::callback). This will be used to forward callbacks registered in @ref usb_can_view_alloc() to @ref SCENE-USB-CAN component. */
void usb_can_view_set_callback(usbCanView* usb_can, usbCanViewCallBack_t callback, void* context) {
    furi_assert(usb_can);
    furi_assert(callback);

    with_view_model_cpp(
        usb_can->view,
        UsbCanModel*,
        model,
        {
            UNUSED(model);
            usb_can->callback = callback;
            usb_can->context = context;
        },
        false);
}

/** @brief function called by @ref usb_can_scene_usb_can_on_event() periodically to update data display (data updated to correspond to those holded by @ref MODEL component) 
 * @details This function calls @ref with_view_model_cpp() macro in order to:
 * -# get pointer to displayed data by calling @ref view_get_model()
 * -# update data pointed by obtained pointer in order to correspond to values supplied in function arguments wich are coming from @ref MODEL component in @ref UsbCanApp::usb_can_bridge @ref UsbCanBridge::cfg and UsbCanBridge::st fields
 * -# tell furi gui component to update displayed values through @ref view_commit_model() function
*/
void usb_can_view_update_state(usbCanView* instance, UsbCanConfig* cfg, UsbCanStatus* st) {
    furi_assert(instance);
    furi_assert(cfg);
    furi_assert(st);

    with_view_model_cpp(
        instance->view,
        UsbCanModel*,
        model,
        {
            model->vcp_port = cfg->vcp_ch;
            model->tx_active = (model->tx_cnt != st->tx_cnt);
            model->rx_active = (model->rx_cnt != st->rx_cnt);
            model->tx_cnt = st->tx_cnt;
            model->rx_cnt = st->rx_cnt;
        },
        true);
}
