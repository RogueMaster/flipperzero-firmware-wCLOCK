/** 
 * @file usbCan_scene.cpp
 * @brief scene definitions : This is the logic between different menus of the applications. These scenes permit to bind their corresponding view (cf. @ref VIEW, in particular usb CAN view) to @ref MODEL component.
 * @ingroup CONTROLLER
 */
#include "usbCan_scene.hpp"

/**  Generate scene on_enter handlers array to be passed @ref SceneManagerHandlers::on_enter_handlers. It fills this array with function names generated in @ref usbCan_scene.hpp.*/
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const usb_can_scene_on_enter_handlers[])(void*) = {
#include "usbCan_scene_config.hpp"
};
#undef ADD_SCENE

/** Generate scene on_event handlers array to be passed @ref SceneManagerHandlers::on_event_handlers. It fills this array with function names generated in @ref usbCan_scene.hpp.*/
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const usb_can_scene_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "usbCan_scene_config.hpp"
};
#undef ADD_SCENE

/** Generate scene on_exit handlers array to be passed @ref SceneManagerHandlers::on_exit_handlers. It fills this array with function names generated in @ref usbCan_scene.hpp.*/
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const usb_can_scene_on_exit_handlers[])(void* context) = {
#include "usbCan_scene_config.hpp"
};
#undef ADD_SCENE

/** Initialize scene handlers configuration structure to be passed to @ref scene_manager_alloc*/
const SceneManagerHandlers usb_can_scene_handlers = {
    .on_enter_handlers = usb_can_scene_on_enter_handlers,
    .on_event_handlers = usb_can_scene_on_event_handlers,
    .on_exit_handlers = usb_can_scene_on_exit_handlers,
    .scene_num = usb_canSceneNum,
};
