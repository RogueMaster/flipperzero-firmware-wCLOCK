#include "usb_can_app_start.h"
/** @brief Application Entry point (registered in app manifest) it calls  @ref usb_can_app.*/
int32_t usb_can_app_start(void* p) {
    usb_can_app(p);
    return 0;
}
