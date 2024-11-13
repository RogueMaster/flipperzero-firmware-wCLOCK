/**
 * @defgroup SCENE-CONFIG
 * @brief This is the scene displayed when left key is pressed in @ref SCENE-USB-CAN : it permits to chose Virtual COM port number.
 * @ingroup CONTROLLER
 * @{
 * @file usbCan_scene_config_vcp.cpp
 * @brief Implements @ref SCENE-CONFIG
 *
 */
#include "../usb_can_bridge.hpp"
#include "../usb_can_app_i.hpp"
#include <furi_hal.h>

typedef enum { UsbUartLineIndexVcp } LineIndex;

/** @brief VCP channel to be proposed to the user. These are the values proposed to user when "USB channel" item is selected.*/
static const char* vcp_ch[] = {"0 (CLI)", "1"};
/** @brief value used to show selected channel (obtained from @ref usb_can_get_config()) on view, and to set configuration selected by the user to @ref MODEL through @ref usb_can_set_config(). */
static UsbCanConfig* SceneConfigToSet;

/** @brief  This callback is trigged by @ref line_vcp_cb on VCP channel selection : it is used to communicate selected channel ( @ref SceneConfigToSet) to @ref MODEL through @ref usb_can_set_config*/
bool usb_can_scene_config_on_event(void* context, SceneManagerEvent event) {
    UsbCanApp* app = (UsbCanApp*)context;
    furi_assert(app);
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UsbCanCustomEventConfig) {
            usb_can_set_config(app->usb_can_bridge, SceneConfigToSet);
            return true;
        }
    }
    return false;
}

/** @brief callback registered through @ref usb_can_scene_config_on_enter() trigged when a VCP channel is choosen by the user. It will trig @ref usb_can_scene_config_on_event().
 *  @details This function performs the following actions:
 * -# get application holder ( @ref UsbCanApp) by calling variable_item_get_context() and passing @ref item parameter.
 * -# get chosen VCP channel through @ref variable_item_get_current_value_index()
 * -# set text to display by calling @ref variable_item_set_current_value_text()
 * -# set @ref SceneConfigToSet according to VCP channel value chosen by the user.
 * -# Trig @ref view_dispatcher_send_custom_event() by sending @ref UsbCanCustomEventConfig to view dispatcher in order to communicate @ref SceneConfigToSet to @ref MODEL.
  */
static void line_vcp_cb(VariableItem* item) {
    UsbCanApp* app = (UsbCanApp*)variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, vcp_ch[index]);

    SceneConfigToSet->vcp_ch = index;
    view_dispatcher_send_custom_event(app->views.view_dispatcher, UsbCanCustomEventConfig);
}

/** @brief This function is called via @ref usb_can_scene_usb_can_on_event() when left arrow is pressed. It will display Virtual Com Port configuration menu items.
 * @details This function perform the following actions:
 * -# get configuration used by @ref MODEL via  @ref usb_can_get_config().
 * -# create the only item for the top level item list : USB VCP channel selection ("USB Channel") by calling @ref variable_item_list_add(). @ref line_vcp_cb callback is registered to perform actions on channel selection (i.e on action on this item).
 * -# set value of selected VCP channel ( @ref VariableItem obtained by calling variable_item_list_add) from value got from @ref MODEL previously through @ref variable_item_set_current_value_index() and @ref variable_item_set_current_value_text() (the text to be displayed to represent variable item value).  
 * -# set pre-selected list item (highlight). It is obviously VCP channel used because it is the only variable item of the previously created variable item list.
 * -# request content described above to be displayed via @ref view_dispatcher_switch_to_view()
 */
void usb_can_scene_config_on_enter(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    furi_assert(app);
    VariableItemList* var_item_list = app->views.menu;

    SceneConfigToSet = (UsbCanConfig*)malloc(sizeof(UsbCanConfig));
    usb_can_get_config(app->usb_can_bridge, SceneConfigToSet);

    VariableItem* item;

    item = variable_item_list_add(var_item_list, "USB Channel", 2, line_vcp_cb, app);
    variable_item_set_current_value_index(item, SceneConfigToSet->vcp_ch);
    variable_item_set_current_value_text(item, vcp_ch[SceneConfigToSet->vcp_ch]);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UsbCanSceneConfig));

    view_dispatcher_switch_to_view(app->views.view_dispatcher, UsbCanAppViewMenu);
}

/** @brief This function is called when leaving @ref SCENE-CONFIG to save scene state (last item selected) via @ref  scene_manager_set_scene_state() and clean view ( free ressources) via @ref variable_item_list_reset(). */
void usb_can_scene_config_on_exit(void* context) {
    UsbCanApp* app = (UsbCanApp*)context;
    scene_manager_set_scene_state(
        app->scene_manager,
        UsbCanSceneConfig,
        variable_item_list_get_selected_item_index(app->views.menu));
    variable_item_list_reset(app->views.menu);
    free(SceneConfigToSet);
}
/*@}*/
