
/** 
 * @defgroup APP
 * @brief Applicative Software : this is an adaptation of Xtreme firmware USB-UART bridge App in order to send data on CAN bus instead of UART.
 * @{
  @defgroup MODEL
  @brief Main application files used to operate USB VCP and CAN Link through @ref CAN-DRIVER module.
  @{
  @file usb_can_bridge.hpp
  @brief contains CAN bus and and USB VCP logic control functions (implemented in usb_can_bridge.cpp) to be exposed.
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <api_lock.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>
#include "usb_can_app_i.hpp"

UsbCanBridge* usb_can_enable(UsbCanApp* app, usbCanState state);
void usb_can_disable(UsbCanBridge* usb_can);
void usb_can_set_config(UsbCanBridge* usb_can, UsbCanConfig* cfg);
void usb_can_get_config(UsbCanBridge* usb_can, UsbCanConfig* cfg);
void usb_can_get_state(UsbCanBridge* usb_can, UsbCanStatus* st);

/** @}*/
