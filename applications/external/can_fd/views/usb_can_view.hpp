
#pragma once

#include <gui/view.h>

#include "../usb_can_app_i.hpp"

usbCanView* usb_can_view_alloc();

void usb_can_view_free(usbCanView* usb_uart);

View* usb_can_get_view(usbCanView* usb_uart);

void usb_can_view_set_callback(usbCanView* usb_uart, usbCanViewCallBack_t callback, void* context);

void usb_can_view_update_state(usbCanView* instance, UsbCanConfig* cfg, UsbCanStatus* st);
