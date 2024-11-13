/**
 * @defgroup SCENE-EXIT-CONFIRM
 * @brief This is the scene displayed when back key is pressed in @ref SCENE-USB-CAN.
 * @ingroup CONTROLLER
 * @{
 * @file usbCan_scene_exit_confirm.cpp
 * @brief Implements @ref SCENE-EXIT-CONFIRM
 *
 */
#include "../usb_can_app_i.hpp"

/** @brief This callback registered in @ref usb_can_scene_exit_confirm_on_enter() is called by furi gui component (DialogEx) when an option is chosen ("Stay" or "Exit"). It will trig @ref usb_can_scene_exit_confirm_on_event(). */
void usb_can_scene_exit_confirm_dialog_callback(DialogExResult result, void* context) {
    UsbCanApp* app = (UsbCanApp*)context;

    view_dispatcher_send_custom_event(app->views.view_dispatcher, result);
}

/** @brief This function is called in @ref SCENE-USB-CAN when user presses back key when in @ref SCENE-USB-CAN.
 * @details This function will display exit confirm message by :
 * -# registering app context to be passed to @ref usb_can_scene_exit_confirm_dialog_callback() by calling @ref dialog_ex_set_context()
 * -# call @ref dialog_ex_set_left_button_text() to create "Exit" button
 * -# call @ref dialog_ex_set_right_button_text() to create "Stay" button 
 * -# create exit message (""Exit USB-CAN?") by calling @ref dialog_ex_set_header()
 * -# register @ref usb_can_scene_exit_confirm_dialog_callback() callback to be called when an oprion (button) is choosen through @ref dialog_ex_set_result_callback()
 * -# request gui component to previously described element by calling @ref view_dispatcher_switch_to_view()
 */
void usb_can_scene_exit_confirm_on_enter(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    DialogEx* dialog = app->views.quit_dialog;

    dialog_ex_set_context(dialog, app);
    dialog_ex_set_left_button_text(dialog, "Exit");
    dialog_ex_set_right_button_text(dialog, "Stay");
    dialog_ex_set_header(dialog, "Exit USB-CAN?", 22, 12, AlignLeft, AlignTop);
    dialog_ex_set_result_callback(dialog, usb_can_scene_exit_confirm_dialog_callback);

    view_dispatcher_switch_to_view(app->views.view_dispatcher, UsbCanAppViewExitConfirm);
}

/** @brief This function is trigged by @ref usb_can_scene_exit_confirm_dialog_callback when an option is chosen ("Stay" or "Exit") to switch to next scene regarding option chosen ( @ref SCENE-START or @ref SCENE-USB-CAN).
 * @details This function will chose scene to go regarding event ( @ref SceneManagerEvent) that trigged the function :
 * - @ref SCENE-START if "Exit" Button was chosen : @ref SceneManagerEvent::event = @ref DialogExResultRight ( @ref SceneManagerEvent::type = @ref SceneManagerEventTypeCustom)
 * - @ref SCENE-USB-CAN if "Stay" button was chosen : @ref SceneManagerEvent::event = @ref DialogExResultLeft
 * - stay in @ref SCENE-EXIT-CONFIRM if back key was pressed : @ref SceneManagerEvent::type = @ref SceneManagerEventTypeBack
 * @return True if back key was pressed (to prevent scene manager automatically restore @ref SCENE-START), False otherwise
 */
bool usb_can_scene_exit_confirm_on_event(void* context, SceneManagerEvent event) {
    UsbCanApp* app = (UsbCanApp*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultRight) {
            consumed = scene_manager_previous_scene(app->scene_manager);
        } else if(event.event == DialogExResultLeft) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, UsbCanSceneStart);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

/** @brief This function is called when leaving @ref SCENE-EXIT-CONFIRM to clean view (and free ressources) via @ref dialog_ex_reset(). */
void usb_can_scene_exit_confirm_on_exit(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;

    // Clean view
    dialog_ex_reset(app->views.quit_dialog);
}
/*@}*/