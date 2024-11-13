/** @file usb_can_app.cpp
 * @brief Application main file. It is top level file and application entry (and exit) point. It allocates and centralize top level ressources, initialize applications components 
 * @ingroup APP
*/
#include "usb_can_app_i.hpp"
#include <furi.h>
#include <furi_hal.h>
#include "views/usb_can_view.hpp"

/** @brief  called on event */
static bool usb_can_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    UsbCanApp* app = (UsbCanApp*)context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

/** @brief  called when back key is pressed by user event: @ref scene_manager_handle_back_event() will be called. */
static bool usb_can_app_back_event_callback(void* context) {
    furi_assert(context);
    UsbCanApp* app = (UsbCanApp*)context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/** @brief  called when tick event is trigged: it will update display through @ref scene_manager_handle_tick_event() will be called. */
static void usb_can_app_tick_event_callback(void* context) {
    furi_assert(context);
    UsbCanApp* app = (UsbCanApp*)context;
    scene_manager_handle_tick_event(app->scene_manager);
}

/** @brief allocate memory space for an @ref UsbCanApp instance and initialize : it will mainly allocate, all views (standard menus and custom @ref VIEW) and start app by calling @ref scene_manager_next_scene()  
 * @details This function is called on application (selection of CAN-FD-HS application by user from the main menu) entry and performs the following actions:
 * -# allocate space for the main application data instance (of type @ref UsbCanApp) holding all references needed for application components functionning.
 * -# allocate space and initialize scene manager used by @ref CONTROLLER component 
 * -# allocate space and initialize  @ref VIEW component ( @ref UsbCanApp::views):
 * <ol>
 * <li> allocate (via  @ref view_dispatcher_enable_queue) and initialize top level view object @ref UsbCanAppViews::view_dispatcher by enabling message queue ( @ref view_dispatcher_enable_queue) and registering callbacks to furi gui module:
 *  - @ref usb_can_app_custom_event_callback registered through @ref view_dispatcher_set_custom_event_callback()
 *  - @ref usb_can_app_back_event_callback registered through @ref view_dispatcher_set_navigation_event_callback()
 *  - @ref usb_can_app_tick_event_callback registered through @ref view_dispatcher_set_tick_event_callback
 * <li> allocate the two item list menus views ( @ref UsbCanAppViews::menu) used to select app mode and to configure VCP channel used (resp. @ref  UsbCanAppViewMenu and @ref UsbCanAppViewCfg) via @ref variable_item_list_alloc and register it to view dispatcher via @ref view_dispatcher_add_view()
 * <li> allocate error message popup view ( @ref UsbCanAppViews::usb_busy_popup) via @ref widget_alloc and register it to view dispatcher via @ref view_dispatcher_add_view()
 * <li> allocate application specific view imlplemented in @ref VIEW module ( @ref UsbCanAppViews::usb_can) via @ref usb_can_view_alloc() and register it to view dispatcher via @ref view_dispatcher_add_view()
 * <li> allocate quit dialog view ( @ref UsbCanAppViews::quit_dialog) via @ref dialog_ex_alloc(). and register it to view dispatcher via @ref view_dispatcher_add_view()
 * </ol>
 * -# start @ref CONTROLLER component by calling @ref scene_manager_next_scene with parameter set to @ref UsbCanSceneStart in order to display app mode selection menu ( @ref SCENE-START).
*/
UsbCanApp* usb_can_app_alloc() {
    UsbCanApp* app = (UsbCanApp*)malloc(sizeof(UsbCanApp));

    app->views.gui = (Gui*)furi_record_open(RECORD_GUI);

    app->scene_manager = scene_manager_alloc(&usb_can_scene_handlers, app);

    app->views.view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->views.view_dispatcher);
    view_dispatcher_set_event_callback_context(app->views.view_dispatcher, app);

    // register view callbacks
    view_dispatcher_set_custom_event_callback(
        app->views.view_dispatcher, usb_can_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->views.view_dispatcher, usb_can_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->views.view_dispatcher, usb_can_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(
        app->views.view_dispatcher, app->views.gui, ViewDispatcherTypeFullscreen);

    app->notifications = (NotificationApp*)furi_record_open(RECORD_NOTIFICATION);

    // add views

    app->views.menu = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->views.view_dispatcher,
        UsbCanAppViewMenu,
        variable_item_list_get_view(app->views.menu));

    view_dispatcher_add_view(
        app->views.view_dispatcher,
        UsbCanAppViewCfg,
        variable_item_list_get_view(app->views.menu));

    app->views.usb_busy_popup = widget_alloc();
    view_dispatcher_add_view(
        app->views.view_dispatcher,
        UsbCanAppViewUsbBusy,
        widget_get_view(app->views.usb_busy_popup));

    app->views.usb_can = usb_can_view_alloc();
    view_dispatcher_add_view(
        app->views.view_dispatcher, UsbCanAppViewUsbCan, usb_can_get_view(app->views.usb_can));

    app->views.quit_dialog = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->views.view_dispatcher,
        UsbCanAppViewExitConfirm,
        dialog_ex_get_view(app->views.quit_dialog));

    scene_manager_next_scene(app->scene_manager, UsbCanSceneStart);

    return app;
}

/** @brief free allocated memory space and ressources used by @ref UsbCanApp instance allocated and initialized via @ref usb_can_app_alloc()*/
void usb_can_app_free(UsbCanApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->views.view_dispatcher, UsbCanAppViewMenu);
    view_dispatcher_remove_view(app->views.view_dispatcher, UsbCanAppViewCfg);
    view_dispatcher_remove_view(app->views.view_dispatcher, UsbCanAppViewUsbCan);
    view_dispatcher_remove_view(app->views.view_dispatcher, UsbCanAppViewUsbBusy);
    view_dispatcher_remove_view(app->views.view_dispatcher, UsbCanAppViewExitConfirm);
    variable_item_list_free(app->views.menu);
    widget_free(app->views.usb_busy_popup);
    usb_can_view_free(app->views.usb_can);
    dialog_ex_free(app->views.quit_dialog);

    // View dispatcher
    view_dispatcher_free(app->views.view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    free(app);
}

/** @brief  Function called by entry function @ref usb_can_app_start. It will allocate ressource through @ref usb_can_app_alloc, start main application thread through @ref view_dispatcher_run(), and free ressources once applications is terminated via @ref usb_can_app_free(). 
 * @details This intermediary function is used because beeing within a C++ compilation unit, and therefore beeing able to use C++ features.
*/
extern "C" int32_t usb_can_app(void* p) {
    UNUSED(p);
    UsbCanApp* usb_can_app = usb_can_app_alloc();

    view_dispatcher_run(usb_can_app->views.view_dispatcher);

    usb_can_app_free(usb_can_app);

    return 0;
}
