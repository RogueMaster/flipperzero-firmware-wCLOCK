/** 
 * @defgroup SCENE-ERROR-DIALOG 
 * @brief This scene is called when the user tries to enter an application mode (i.e @ref SCENE-USB-CAN) from @ref SCENE-START but USB link is busy.
 * @ingroup CONTROLLER
 * @{
 * @file usbCan_scene_usb_can.cpp
 * @brief Implements @ref SCENE-ERROR-DIALOG
 * @ingroup CONTROLLER
 */

#include "../usb_can_app_i.hpp"
#include "../usb_can_custom_event.hpp"
#include "canfdhs_icons.h"

/** @brief This function is called when @ref  SCENE-ERROR-DIALOG  is entered. It will display 2 popup messages and an icon.
 * @details This function:
 * -# add a connection icon to @ref UsbCanAppViewUsbBusy (see @ref usb_can_app_alloc()) via @ref widget_add_icon_element 
 * -# add 2 string messages to @ref UsbCanAppViewUsbBusy via @ref widget_add_string_multiline_element
 * -# call @ref view_dispatcher_switch_to_view to display above elements 
 */
void usb_can_scene_error_dialog_on_enter(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;

    widget_add_icon_element(app->views.usb_busy_popup, 78, 0, &I_ActiveConnection_50x64);
    widget_add_string_multiline_element(
        app->views.usb_busy_popup,
        3,
        2,
        AlignLeft,
        AlignTop,
        FontPrimary,
        "Connection\nis active!");
    widget_add_string_multiline_element(
        app->views.usb_busy_popup,
        3,
        30,
        AlignLeft,
        AlignTop,
        FontSecondary,
        "Disconnect from\nPC or phone to\nuse this function.");

    view_dispatcher_switch_to_view(app->views.view_dispatcher, UsbCanAppViewUsbBusy);
}

/** @brief This function is called when a key is pressed by the user. If it is back key @ref SCENE-START is re-entered. 
 * @details This function is probably called by scene manager to evaluate if this scene shall be leaved (to return to @ref SCENE-START):
 * If event corresponds to a back key press:
 * -# call @ref scene_manager_previous_scene() to run previous scene (@ref SCENE-START)
 * -# if @ref scene_manager_previous_scene() returned an error (i.e false) : stop app via @ref scene_manager_stop and @ref view_dispatcher_stop
 * @return true if key if back key was entered, false if another event has trigged this function.
 */
bool usb_can_scene_error_dialog_on_event(void* context, SceneManagerEvent event) {
    UsbCanApp* app = (UsbCanApp*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UsbCanCustomEventErrorBack) {
            if(!scene_manager_previous_scene(app->scene_manager)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->views.view_dispatcher);
            }
            consumed = true;
        }
    }
    return consumed;
}

/** @brief This function is called by @ref usb_can_scene_error_dialog_on_event() via scene_manager before leaving @ref SCENE-ERROR-DIALOG. It call @ref widget_reset.*/
void usb_can_scene_error_dialog_on_exit(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    widget_reset(app->views.usb_busy_popup);
}
/*@}*/