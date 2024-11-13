/** @defgroup CONTROLLER
 * @brief MVC controller component used to link @ref VIEW (data displayed in flipper screen) to @ref MODEL (application control logic : USB and CAN link operation)
 * @image html controller.png
 * @ingroup APP
 * @{
 * @file usbCan_scene.hpp
 * @brief scene definitions : This is the logic between different menus of the applications. These scenes permit to bind their corresponding view (cf. @ref VIEW, in particular usb CAN view) to @ref MODEL component. 
 */
#pragma once

#include <gui/scene_manager.h>

/**  Generate scene id, alias and total number. This is called in @ref usbCan_scene_config.hpp in order to generate @ref usb_canScene enum.*/
#define ADD_SCENE(prefix, name, id) UsbCanScene##id,
/** Scene identifiers */
typedef enum {
#include "usbCan_scene_config.hpp"
    usb_canSceneNum, /**< Additionnal identifier used to return the number of defined scene identifiers in @ref usbCan_scene_config.hpp  */
} usb_canScene;
#undef ADD_SCENE

extern const SceneManagerHandlers usb_can_scene_handlers;

/**  Generate scene on_enter handlers declaration. This is called in @ref usbCan_scene_config.hpp.*/
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "usbCan_scene_config.hpp"
#undef ADD_SCENE

/** Generate scene on_event handlers declaration. This is called in @ref usbCan_scene_config.hpp.*/
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "usbCan_scene_config.hpp"
#undef ADD_SCENE

/**  Generate scene on_exit handlers declaration. This is called in @ref usbCan_scene_config.hpp.*/
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "usbCan_scene_config.hpp"
#undef ADD_SCENE
/** @}*/
