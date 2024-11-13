#pragma once
/** 
  @file usb_can_custom_event.hpp
  @ingroup MODEL
  @brief
  contains event related to USB CAN Application definitions.
 */

/** 
    @brief event related to USB CAN Application*/
typedef enum {
    UsbCanCustomEventUsbCan, /**< Used to signal to application it shall run in USB-CAN bridge mode : operationnal main mode.*/
    UsbCanCustomEventTestUsb, /**< Used to signal to application it shall run in USB loopback mode  :test mode in which data received via USB is sent back.*/
    UsbCanCustomEventTestCan, /**< Used to signal to application it shall run in CAN test mode : test mode in which app send 007E5TCA:43414E4C49564500 on can.*/
    UsbCanCustomEventErrorBack,
    UsbCanCustomEventConfig, /**< Used to signal to application it shall configure CAN link.*/
    UsbCanCustomEventConfigSet
} UsbCanCustomEvent;
