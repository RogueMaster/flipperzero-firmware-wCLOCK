/**
 * @defgroup SCENE-START 
 * @brief This is the scene displayed on application entry (when CAN-FD-HS app is selected from flipper main menu).
 * @ingroup CONTROLLER
 * @{
 * @file usbCan_scene_usb_can.cpp
 * @brief Implements @ref SCENE-START
 *
 */
#include "../usb_can_app_i.hpp"
#include <furi_hal_power.h>
#include <furi_hal_usb.h>
#include <dolphin/dolphin.h>
#include "usbCan_scene_setState.hpp"

/** @brief identifier of the item list element (i.e Application mode : USB-CAN-BRIDGE, USB LOOPBACK TEST,TEST CAN) selected by the user and communicated to @ref CONTROLLER via @ref usb_can_scene_start_var_list_enter_callback*/
typedef enum {
    UsbCanItemUsbCan, /** < identifier (0) corresponding to USB-CAN-BRIDGE item*/
    UsbCanItemTestUsb, /** < identifier (1) corresponding to USB LOOPBACK TEST item */
    UsbCanItemTestCan /** < identifier (2) corresponding to TESt CAN TEST item */
} UsbCanItem;

/** @brief Function called when an application mode is entered : this will communicate mode to be entered to @ref MODEL before trying to switch to @ref SCENE-USB-CAN via @ref usb_can_scene_start_on_event().
 * @details Function called when a list item is selected. This was registered in @ref usb_can_scene_start_on_enter().
 *  This function does the fololowing actions:
 * -# save selected item via @ref usb_can_scene_start_var_list_enter_callback to be pre-selected on next @ref SCENE-START entry
 * -# Communicate mode to be entered to @ref MODEL via @ref usb_can_scene_set_state()
 * -# Try to switch to @ref SCENE-USB-CAN by sending a signal (event) to trig @ref usb_can_scene_start_on_event()
*/
static void usb_can_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UsbCanApp* app = (UsbCanApp*)context;
    scene_manager_set_scene_state(app->scene_manager, UsbCanSceneStart, index);
    switch(index) {
    case UsbCanItemTestUsb:
        usb_can_scene_set_state(UsbCanLoopBackTestState);
        view_dispatcher_send_custom_event(app->views.view_dispatcher, UsbCanCustomEventTestUsb);
        break;
    case UsbCanItemTestCan:
        usb_can_scene_set_state(UsbCanPingTestState);
        view_dispatcher_send_custom_event(app->views.view_dispatcher, UsbCanCustomEventTestCan);
        break;
    case UsbCanItemUsbCan:
        usb_can_scene_set_state(UsbCanNominalState);
        view_dispatcher_send_custom_event(app->views.view_dispatcher, UsbCanCustomEventUsbCan);
        break;
    }
}

/** @brief This function is called via @ref usb_can_app_alloc() at app start and via other scenes basically when back key is pressed. It will display main menu items which correspond to application mode (USB-CAN-BRIDGE, USB LOOPBACK TEST,TEST CAN).
 * @details This function perform the following actions:
 * -# set @ref usb_can_scene_start_var_list_enter_callback callback when an item list is selected via @ref variable_item_list_set_enter_callback
 * -# add list item in this order : "USB-CAN Bridge" (0) ,"TEST USB LOOPBACK" (1), "TEST CAN" (2). This is important because it must correspond to @ref UsbCanItem.
 * -# enable power via USB through @ref furi_hal_power_enable_otg() since we can consider flipper is probably connected to USB.
 * -# set pre-selected list item (highlight). It shall be :
 *      - last pre-selected list item if this scene is re-entered 
 *      - USB-CAN-BRIDGE item if the scene is entered for the first time (initial state)
 * -# request content described above to be displayed via @ref view_dispatcher_switch_to_view()
 */
void usb_can_scene_start_on_enter(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    VariableItemList* var_item_list = app->views.menu;

    variable_item_list_set_enter_callback(
        var_item_list, usb_can_scene_start_var_list_enter_callback, app);

    variable_item_list_add(var_item_list, "USB-CAN Bridge", 0, NULL, NULL);

    variable_item_list_add(var_item_list, "TEST USB LOOPBACK", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "TEST CAN", 0, NULL, NULL);

    furi_hal_power_enable_otg();

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UsbCanSceneStart));

    view_dispatcher_switch_to_view(app->views.view_dispatcher, UsbCanAppViewMenu);
}

/** @brief This function is trigged by @ref usb_can_scene_start_var_list_enter_callback(). This will try to run @ref SCENE-USB-CAN if USB Link is avilable, @ref SCENE-ERROR-DIALOG otherwise.
 * @details This function is trigged by @ref usb_can_scene_start_var_list_enter_callback() which send an event through scene_manager
 * -# check event is expected (sent by usb_can_scene_start_var_list_enter_callback : UsbCanCustomEventTestCan, UsbCanCustomEventTestUsb or UsbCanCustomEventUsbCan)
 * -# check usb is available through @ref furi_hal_usb_is_locked()
 *      - if USB is available, go to @ref SCENE-USB-CAN in order to run USB and CAN ling ( @ref MODEL )
 *      - Otherwise it will run @ref SCENE-ERROR-DIALOG
 */
bool usb_can_scene_start_on_event(void* context, SceneManagerEvent event) {
    UsbCanApp* app = (UsbCanApp*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case UsbCanCustomEventTestCan:
        case UsbCanCustomEventTestUsb:
        case UsbCanCustomEventUsbCan:
            if(!furi_hal_usb_is_locked()) {
                //dolphin_deed(DolphinDeedusb_canUartBridge);
                scene_manager_next_scene(app->scene_manager, UsbCanSceneUsbCan);
            } else {
                scene_manager_next_scene(app->scene_manager, UsbCanSceneErrorDialog);
            }
            consumed = true;
            break;
        }
    }
    return consumed;
}

/** @brief This function is called when back key is pressed. It will call @ref variable_item_list_reset. */
void usb_can_scene_start_on_exit(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    variable_item_list_reset(app->views.menu);
}
/*@}*/