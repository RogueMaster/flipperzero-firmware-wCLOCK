/** 
 * @defgroup SCENE-USB-CAN 
 * @brief This is the main scene used when an application mode is entered. It will mainly run @ref MODEL component and display related informations using  @ref VIEW component.
 * @ingroup CONTROLLER
 * @{
 * @file usbCan_scene_usb_can.cpp
 * @brief Implements @ref SCENE-USB-CAN
 * @ingroup CONTROLLER
 */
#include "../usb_can_app_i.hpp"
#include "../usb_can_bridge.hpp"
#include "usbCan_scene.hpp"
#include "../views/usb_can_view.hpp"

/** This is the application mode requested by the user through @ref usb_can_scene_set_state. This requested state will be communicated to @ref MODEL component through @ref usb_can_scene_usb_can_on_enter function.*/
usbCanState UsbCanStateToSet;

/** 
 * @brief This function is called by @ref usb_can_scene_start_var_list_enter_callback when an application mode is entered. It set @ref UsbCanStateToSet.
 * @details This function is called by @ref usb_can_scene_start_var_list_enter_callback when an application mode (USB-LOOPBACK-TEST, USB-CAN-BRIGE, CAN-TEST) by the user in the application main menu.
 */
void usb_can_scene_set_state(usbCanState state) {
    UsbCanStateToSet = state;
}

/** 
 * @brief This function is a callback registered in @ref VIEW module via @ref usb_can_scene_usb_can_on_enter.
 * @details This function is a callback registered in @ref VIEW module via @ref usb_can_scene_usb_can_on_enter used to perform actions when events are sent by furi gui component.
 * It basically forwards arguments to view dispatcher @ref UsbCanApp::views -> @ref UsbCanAppViews::view_dispatcher via @ref view_dispatcher_send_custom_event.
 * Then view dispatcher will call back @ref usb_can_scene_usb_can_on_event to process event.
 */
void usb_can_scene_usb_can_callback(UsbCanCustomEvent event, void* context) {
    furi_assert(context);
    UsbCanApp* app = (UsbCanApp*)context;
    view_dispatcher_send_custom_event(app->views.view_dispatcher, event);
}

/** 
 * @brief This function is called when an application mode is selected by the user in the application main menu. It mainly runs application logic by calling @ref usb_can_enable().
 * @details This function is called by @ref usb_can_scene_start_on_event() when an application mode (USB-LOOPBACK-TEST, USB-CAN-BRIGE, CAN-TEST) is selected by the user in the application main menu. 
 * It performs the following actions:
 * -# call @ref usb_can_enable() passing @ref UsbCanStateToSet to set application mode in @ref MODEL.
 * -# register @ref usb_can_scene_usb_can_callback callback in furi gui component via @ref usb_can_view_set_callback
 * -# request @ref VIEW component to display USB CAN App custom view ( @ref UsbCanAppViewUsbCan) by calling @ref view_dispatcher_switch_to_view()
 * -# call @ref notification_message (with @ref sequence_display_backlight_enforce_on argument)
 */
void usb_can_scene_usb_can_on_enter(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;

    usb_can_enable(app, UsbCanStateToSet);
    usb_can_view_set_callback(app->views.usb_can, usb_can_scene_usb_can_callback, app);
    view_dispatcher_switch_to_view(app->views.view_dispatcher, UsbCanAppViewUsbCan);
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
}

/** 
 * @brief Events are processed here : It will either go to config state or exit application running mode on depending on user input or refresh display.
 * @details Events forwarded by @ref usb_can_scene_usb_can_callback through view dispatcher are processed here : 
 * - If user press left key : Enter in @ref SCENE-CONFIG (and view) to configure VCP channel by calling @ref scene_manager_next_scene with @ref UsbCanSceneConfig argument.
 * - If back key is pressed by the user : Exit this scene and stop @ref MODEL component via @ref usb_can_scene_usb_can_on_exit() by switching to @ref usbCan_scene_exit_confirm by calling @ref scene_manager_next_scene with @ref UsbCanSceneExitConfirm argument.
 * - If a tick event is signaled : refresh @ref MODEL informations displayed to user by calling @ref usb_can_view_update_state
 */
bool usb_can_scene_usb_can_on_event(void* context, SceneManagerEvent event) {
    UsbCanApp* app = (UsbCanApp*)context;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, UsbCanSceneConfig);
        return true;
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(app->scene_manager, UsbCanSceneExitConfirm);
        return true;
    } else if(event.type == SceneManagerEventTypeTick) {
        /*uint32_t tx_cnt_last = scene_usb_can->status.tx_cnt;
        uint32_t rx_cnt_last = scene_usb_can->status.rx_cnt;*/
        usb_can_view_update_state(
            app->views.usb_can, &app->usb_can_bridge->cfg_new, &app->usb_can_bridge->st);
        /*if(tx_cnt_last != scene_usb_can->state.tx_cnt)
            notification_message(app->notifications, &sequence_blink_blue_10);
        if(rx_cnt_last != scene_usb_can->state.rx_cnt)
            notification_message(app->notifications, &sequence_blink_green_10);*/
    }
    return false;
}

/** @brief This function is called by @ref usb_can_scene_usb_can_on_event() via scene_manager and is used to stop @ref MODEL component via @ref usb_can_disable().*/
void usb_can_scene_usb_can_on_exit(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    usb_can_disable(app->usb_can_bridge);
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
}
/*@}*/