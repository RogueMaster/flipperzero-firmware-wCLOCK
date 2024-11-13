/** @brief @file usb_can_app_i.hpp
 * @brief Application top level header file. It holds main structures definitions related to @ref MODEL, @ref VIEW and @ref CONTROLLER.
 * @ingroup APP
*/
#pragma once
#include "scenes/usbCan_scene.hpp"
#include "usb_can_custom_event.hpp"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/dialog_ex.h>
#include "usb_can_custom_event.hpp"
#include "Longan_CANFD/src/mcp2518fd_can.h"
#include "api_lock.h"
#include "furi_hal_usb_cdc.h"
#define USB_CDC_PKT_LEN CDC_DATA_SZ

/** @brief Configuration used by @ref MODEL to configure USB CDC VCP communication */
typedef struct {
    uint8_t
        vcp_ch; /**< Virtual COM port channel used in flipper zero. It can be used to open an additionnal VCP to flipper CLI. **/
} UsbCanConfig;
typedef void (*usbCanViewCallBack_t)(UsbCanCustomEvent event, void* context);

/** @brief Data used by @ref VIEW component. */
typedef struct {
    View* view; /**< view object to be passed to furi gui functions*/
    usbCanViewCallBack_t callback; /**< hold callback registered by @ref CONTROLLER component */
    void* context; /**< hold pointer registered by @ref CONTROLLER component to be passed to @ref usbCanView::callback*/
} usbCanView;

/** @brief @ref MODEL atatus information to be displayed by  @ref VIEW component */
typedef struct {
    uint32_t rx_cnt; /**< received bytes on CAN Link */
    uint32_t
        tx_cnt; /**< transmitted bytes  : on CAN Link when in USB-CAN bridge mode or on USB link in TEST-USB mode*/
} UsbCanStatus;

/** @brief Correspond to application mode selected by the user (USB-CAN-BRIDGE, USB-LOOPBACK-TEST, TEST-CAN). @ref MODEL component compare UsbCanBridge::st to these values to define its behavior.*/
typedef enum {
    UsbCanLoopBackTestState, /**<  USB-LOOPBACK-TEST*/
    UsbCanPingTestState, /**< TEST-CAN */
    UsbCanNominalState /**< USB-CAN-BRIDGE (main mode)*/
} usbCanState;

/** @brief Holds @ref MODEL component operationnal data and ressources (threads, mutexes,timer)*/
typedef struct {
    UsbCanConfig
        cfg; /**< effective configuration. It shall be updated to correspond to cfg_new whenever possible.*/
    UsbCanConfig cfg_new; /**< configuration to be applied (requested by user)*/
    usbCanState
        state; /**< it correspond to the current application mode selected by user (USB-CAN-BRIDGE, USB-LOOPBACK-TEST, TEST-CAN) */
    mcp2518fd* can; /**< CAN instance used to operate @ref CAN-DRIVER component */
    FuriThread* thread; /**< holds a pointer to application main thread ( @ref usb_can_worker) */
    FuriThread*
        tx_thread; /**< holds a pointer to application CAN TX thread ( @ref usb_can_tx_thread) */
    FuriStreamBuffer*
        rx_stream; /**< holds a pointer to rx buffer used to store USB CDC (VCP) data */
    FuriMutex* tx_mutex; /**< mutex used to synchronize USB CDC driver access */
    UsbCanStatus st; /**< status of the applications to be displayed by @ref VIEW component */
    FuriApiLock cfg_lock; /**< status of the applications to be displayed by @ref VIEW component */
    FuriTimer* timer; /**< timer used to blink CAN TX LED */
    uint8_t rx_buf
        [USB_CDC_PKT_LEN]; /**< USB CDC RX buffer used to store SLCAN commands in USB-CAN-BRIDGE mode or to store data tto be sent back in USB loopback mode */
} UsbCanBridge;

/** @brief Holds @ref VIEW component operationnal data and ressources (furi gui objects) allocated in @ref usb_can_app_alloc*/
typedef struct {
    Gui* gui;
    ViewDispatcher*
        view_dispatcher; /**< view dispatcher used to centralize callbacks and used as an entry point*/
    Widget* usb_busy_popup; /**< popup displayed when usb CDC channel is already in use */
    DialogEx*
        quit_dialog; /**< dialog displayed when user leaves main menu (app mode selection) by pressing back key */
    VariableItemList* menu; /**< main menu (app mode selection) and USB link configuration menu */
    usbCanView*
        usb_can; /**< Operating mode view (used once in USB-CAN-BRIDGE, USB-LOOPBACK-TEST or CAN TEST mode), this is the only structure managed in this app by @ref VIEW component */
} UsbCanAppViews;

/** @brief main holder. This is the Application root structure holding @ref MODEL, @ref VIEW, and @ref CONTROLLER components operationnal ressources and data. */
typedef struct {
    NotificationApp* notifications;
    SceneManager*
        scene_manager; /**< holds @ref CONTROLLER operationnal data and ressources (i.e application control related data used to operate @ref MODEL and @ref VIEW components)*/
    UsbCanAppViews
        views; /**< holds @ref VIEW operationnal data and ressources (i.e GUI related ressources)*/
    UsbCanBridge*
        usb_can_bridge; /**< holds @ref MODEL operationnal data and ressources (i.e USB and CAN link related ressources)*/
} UsbCanApp;

/** @brief Application view identifiers allocated in  */
typedef enum {
    UsbCanAppViewMenu, /**< identifier used for main menu view  (App mode selection menu) */
    UsbCanAppViewCfg, /**< identifier used for USB link configuration menu*/
    UsbCanAppViewUsbCan, /**< identifier used for application specific view implemented in @ref VIEW*/
    UsbCanAppViewUsbBusy, /**< identifier used for error pop up message (appearing when USB CDC link is already in use)  */
    UsbCanAppViewExitConfirm /**< identifier used for exit confirmation message (appearing when user press back key from main menu)*/
} UsbCanAppView;
