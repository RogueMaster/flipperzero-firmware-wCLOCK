/*******************************************************************************
  CAN FD SPI Driver: Register Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_canfdspi_register.h

  Summary:
    This header file contains SPI instruction defines, register address defines,
    register structures, and reset values of registers.

  Description:
    This file is used by the API.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software and
any derivatives exclusively with Microchip products. It is your responsibility
to comply with third party license terms applicable to your use of third party
software (including open source software) that may accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES
OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER
RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF
THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED
BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO
THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID
DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _DRV_CANFDSPI_REGISTER_H
#define _DRV_CANFDSPI_REGISTER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files

#include "Arduino.h"
#include <inttypes.h>
#include "mcp_can_fd.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
/* SPI Instruction Set */

#define cINSTRUCTION_RESET      0x00
#define cINSTRUCTION_READ       0x03
#define cINSTRUCTION_READ_CRC   0x0B
#define cINSTRUCTION_WRITE      0x02
#define cINSTRUCTION_WRITE_CRC  0x0A
#define cINSTRUCTION_WRITE_SAFE 0x0C

// *****************************************************************************
// *****************************************************************************
/* Register Addresses */

/* CAN FD Controller */
#define cREGADDR_CiCON    0x000
#define cREGADDR_CiNBTCFG 0x004
#define cREGADDR_CiDBTCFG 0x008
#define cREGADDR_CiTDC    0x00C

#define cREGADDR_CiTBC       0x010
#define cREGADDR_CiTSCON     0x014
#define cREGADDR_CiVEC       0x018
#define cREGADDR_CiINT       0x01C
#define cREGADDR_CiINTFLAG   cREGADDR_CiINT
#define cREGADDR_CiINTENABLE (cREGADDR_CiINT)

#define cREGADDR_CiRXIF   0x020
#define cREGADDR_CiTXIF   0x024
#define cREGADDR_CiRXOVIF 0x028
#define cREGADDR_CiTXATIF 0x02C

#define cREGADDR_CiTXREQ  0x030
#define cREGADDR_CiTREC   0x034
#define cREGADDR_CiBDIAG0 0x038
#define cREGADDR_CiBDIAG1 0x03C

#define cREGADDR_CiTEFCON 0x040
#define cREGADDR_CiTEFSTA 0x044
#define cREGADDR_CiTEFUA  0x048
#define cREGADDR_CiFIFOBA 0x04C

#define cREGADDR_CiFIFOCON 0x050
#define cREGADDR_CiFIFOSTA 0x054
#define cREGADDR_CiFIFOUA  0x058
#define CiFIFO_OFFSET      (3 * 4)

#define cREGADDR_CiTXQCON 0x050
#define cREGADDR_CiTXQSTA 0x054
#define cREGADDR_CiTXQUA  0x058

// The filters start right after the FIFO control/status registers
#define cREGADDR_CiFLTCON \
    (cREGADDR_CiFIFOCON + (CiFIFO_OFFSET * CAN_FIFO_TOTAL_CHANNELS)) // 0x50+12*32
#define cREGADDR_CiFLTOBJ (cREGADDR_CiFLTCON + CAN_FIFO_TOTAL_CHANNELS)
#define cREGADDR_CiMASK   (cREGADDR_CiFLTOBJ + 4)

#define CiFILTER_OFFSET (2 * 4)

/* MCP25xxFD Specific */
#define cREGADDR_OSC    0xE00
#define cREGADDR_IOCON  0xE04
#define cREGADDR_CRC    0xE08
#define cREGADDR_ECCCON 0xE0C
#define cREGADDR_ECCSTA 0xE10
#ifndef MCP2517FD
#define cREGADDR_DEVID 0xE14
#endif

/* RAM addresses */
#define cRAM_SIZE 2048

#define cRAMADDR_START 0x400
#define cRAMADDR_END   (cRAMADDR_START + cRAM_SIZE)

/* SPI Chip Select */
#define MCP2518fd_SELECT()  \
    pinMode(SPICS, OUTPUT); \
    digitalWrite(SPICS, LOW)
#define MCP2518fd_UNSELECT() \
    pinMode(SPICS, OUTPUT);  \
    digitalWrite(SPICS, HIGH)

// *****************************************************************************
// *****************************************************************************
/* Register Structures */

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Implementation

// Device selection
//#define MCP2517FD
#define MCP2518FD

// Maximum Size of TX/RX Object
#define MAX_MSG_SIZE 76

// Maximum number of data bytes in message
#define MAX_DATA_BYTES 64

// *****************************************************************************
// *****************************************************************************
// Section: Object definitions

//! CAN FIFO Channels

typedef enum {
    CAN_FIFO_CH0, // CAN_TXQUEUE_CH0
    CAN_FIFO_CH1,
    CAN_FIFO_CH2,
    CAN_FIFO_CH3,
    CAN_FIFO_CH4,
    CAN_FIFO_CH5,
    CAN_FIFO_CH6,
    CAN_FIFO_CH7,
    CAN_FIFO_CH8,
    CAN_FIFO_CH9,
    CAN_FIFO_CH10,
    CAN_FIFO_CH11,
    CAN_FIFO_CH12,
    CAN_FIFO_CH13,
    CAN_FIFO_CH14,
    CAN_FIFO_CH15,
    CAN_FIFO_CH16,
    CAN_FIFO_CH17,
    CAN_FIFO_CH18,
    CAN_FIFO_CH19,
    CAN_FIFO_CH20,
    CAN_FIFO_CH21,
    CAN_FIFO_CH22,
    CAN_FIFO_CH23,
    CAN_FIFO_CH24,
    CAN_FIFO_CH25,
    CAN_FIFO_CH26,
    CAN_FIFO_CH27,
    CAN_FIFO_CH28,
    CAN_FIFO_CH29,
    CAN_FIFO_CH30,
    CAN_FIFO_CH31,
    CAN_FIFO_TOTAL_CHANNELS
} CAN_FIFO_CHANNEL;

// FIFO0 is a special FIFO, the TX Queue
#define CAN_TXQUEUE_CH0 CAN_FIFO_CH0

//! CAN Filter Channels

typedef enum {
    CAN_FILTER0,
    CAN_FILTER1,
    CAN_FILTER2,
    CAN_FILTER3,
    CAN_FILTER4,
    CAN_FILTER5,
    CAN_FILTER6,
    CAN_FILTER7,
    CAN_FILTER8,
    CAN_FILTER9,
    CAN_FILTER10,
    CAN_FILTER11,
    CAN_FILTER12,
    CAN_FILTER13,
    CAN_FILTER14,
    CAN_FILTER15,
    CAN_FILTER16,
    CAN_FILTER17,
    CAN_FILTER18,
    CAN_FILTER19,
    CAN_FILTER20,
    CAN_FILTER21,
    CAN_FILTER22,
    CAN_FILTER23,
    CAN_FILTER24,
    CAN_FILTER25,
    CAN_FILTER26,
    CAN_FILTER27,
    CAN_FILTER28,
    CAN_FILTER29,
    CAN_FILTER30,
    CAN_FILTER31,
    CAN_FILTER_TOTAL,
} CAN_FILTER;

//! CAN Operation Modes

typedef enum {
    CAN_NORMAL_MODE = 0x00,
    CAN_SLEEP_MODE = 0x01,
    CAN_INTERNAL_LOOPBACK_MODE = 0x02,
    CAN_LISTEN_ONLY_MODE = 0x03,
    CAN_CONFIGURATION_MODE = 0x04,
    CAN_EXTERNAL_LOOPBACK_MODE = 0x05,
    CAN_CLASSIC_MODE = 0x06,
    CAN_RESTRICTED_MODE = 0x07,
    CAN_INVALID_MODE = 0xFF
} CAN_OPERATION_MODE;

//! Transmit Bandwidth Sharing

typedef enum {
    CAN_TXBWS_NO_DELAY,
    CAN_TXBWS_2,
    CAN_TXBWS_4,
    CAN_TXBWS_8,
    CAN_TXBWS_16,
    CAN_TXBWS_32,
    CAN_TXBWS_64,
    CAN_TXBWS_128,
    CAN_TXBWS_256,
    CAN_TXBWS_512,
    CAN_TXBWS_1024,
    CAN_TXBWS_2048,
    CAN_TXBWS_4096
} CAN_TX_BANDWITH_SHARING;

//! Wake-up Filter Time

typedef enum {
    CAN_WFT00,
    CAN_WFT01,
    CAN_WFT10,
    CAN_WFT11
} CAN_WAKEUP_FILTER_TIME;

//! Data Byte Filter Number

typedef enum {
    CAN_DNET_FILTER_DISABLE = 0,
    CAN_DNET_FILTER_SIZE_1_BIT,
    CAN_DNET_FILTER_SIZE_2_BIT,
    CAN_DNET_FILTER_SIZE_3_BIT,
    CAN_DNET_FILTER_SIZE_4_BIT,
    CAN_DNET_FILTER_SIZE_5_BIT,
    CAN_DNET_FILTER_SIZE_6_BIT,
    CAN_DNET_FILTER_SIZE_7_BIT,
    CAN_DNET_FILTER_SIZE_8_BIT,
    CAN_DNET_FILTER_SIZE_9_BIT,
    CAN_DNET_FILTER_SIZE_10_BIT,
    CAN_DNET_FILTER_SIZE_11_BIT,
    CAN_DNET_FILTER_SIZE_12_BIT,
    CAN_DNET_FILTER_SIZE_13_BIT,
    CAN_DNET_FILTER_SIZE_14_BIT,
    CAN_DNET_FILTER_SIZE_15_BIT,
    CAN_DNET_FILTER_SIZE_16_BIT,
    CAN_DNET_FILTER_SIZE_17_BIT,
    CAN_DNET_FILTER_SIZE_18_BIT
} CAN_DNET_FILTER_SIZE;

//! FIFO Payload Size

typedef enum {
    CAN_PLSIZE_8,
    CAN_PLSIZE_12,
    CAN_PLSIZE_16,
    CAN_PLSIZE_20,
    CAN_PLSIZE_24,
    CAN_PLSIZE_32,
    CAN_PLSIZE_48,
    CAN_PLSIZE_64
} CAN_FIFO_PLSIZE;

//! CAN Configure

typedef struct _CAN_CONFIG {
    uint32_t DNetFilterCount               : 5;
    uint32_t IsoCrcEnable                  : 1;
    uint32_t ProtocolExpectionEventDisable : 1;
    uint32_t WakeUpFilterEnable            : 1;
    uint32_t WakeUpFilterTime              : 2;
    uint32_t BitRateSwitchDisable          : 1;
    uint32_t RestrictReTxAttempts          : 1;
    uint32_t EsiInGatewayMode              : 1;
    uint32_t SystemErrorToListenOnly       : 1;
    uint32_t StoreInTEF                    : 1;
    uint32_t TXQEnable                     : 1;
    uint32_t TxBandWidthSharing            : 4;
} CAN_CONFIG;

//! CAN Transmit Channel Configure

typedef struct _CAN_TX_FIFO_CONFIG {
    uint32_t RTREnable   : 1;
    uint32_t TxPriority  : 5;
    uint32_t TxAttempts  : 2;
    uint32_t FifoSize    : 5;
    uint32_t PayLoadSize : 3;
} CAN_TX_FIFO_CONFIG;

//! CAN Transmit Queue Configure

typedef struct _CAN_TX_QUEUE_CONFIG {
    uint32_t TxPriority  : 5;
    uint32_t TxAttempts  : 2;
    uint32_t FifoSize    : 5;
    uint32_t PayLoadSize : 3;
} CAN_TX_QUEUE_CONFIG;

//! CAN Receive Channel Configure

typedef struct _CAN_RX_FIFO_CONFIG {
    uint32_t RxTimeStampEnable : 1;
    uint32_t FifoSize          : 5;
    uint32_t PayLoadSize       : 3;
} CAN_RX_FIFO_CONFIG;

//! CAN Transmit Event FIFO Configure

typedef struct _CAN_TEF_CONFIG {
    uint32_t TimeStampEnable : 1;
    uint32_t FifoSize        : 5;
} CAN_TEF_CONFIG;

/* CAN Message Objects */

//! CAN Message Object ID

typedef struct _CAN_MSGOBJ_ID {
    uint32_t SID            : 11;
    uint32_t EID            : 18;
    uint32_t SID11          : 1;
    uint32_t unimplemented1 : 2;
} CAN_MSGOBJ_ID;

//! CAN Data Length Code

typedef enum {
    CAN_DLC_0,
    CAN_DLC_1,
    CAN_DLC_2,
    CAN_DLC_3,
    CAN_DLC_4,
    CAN_DLC_5,
    CAN_DLC_6,
    CAN_DLC_7,
    CAN_DLC_8,
    CAN_DLC_12,
    CAN_DLC_16,
    CAN_DLC_20,
    CAN_DLC_24,
    CAN_DLC_32,
    CAN_DLC_48,
    CAN_DLC_64
} CAN_DLC;

//! CAN TX Message Object Control

typedef struct _CAN_TX_MSGOBJ_CTRL {
    uint32_t DLC : 4;
    uint32_t IDE : 1;
    uint32_t RTR : 1;
    uint32_t BRS : 1;
    uint32_t FDF : 1;
    uint32_t ESI : 1;
#ifdef MCP2517FD
    uint32_t SEQ            : 7;
    uint32_t unimplemented1 : 16;
#else
    uint32_t SEQ : 23;
#endif
} CAN_TX_MSGOBJ_CTRL;

//! CAN RX Message Object Control

typedef struct _CAN_RX_MSGOBJ_CTRL {
    uint32_t DLC            : 4;
    uint32_t IDE            : 1;
    uint32_t RTR            : 1;
    uint32_t BRS            : 1;
    uint32_t FDF            : 1;
    uint32_t ESI            : 1;
    uint32_t unimplemented1 : 2;
    uint32_t FilterHit      : 5;
    uint32_t unimplemented2 : 16;
} CAN_RX_MSGOBJ_CTRL;

//! CAN Message Time Stamp
typedef uint32_t CAN_MSG_TIMESTAMP;

//! CAN TX Message Object

typedef union _CAN_TX_MSGOBJ {
    struct {
        CAN_MSGOBJ_ID id;
        CAN_TX_MSGOBJ_CTRL ctrl;
        CAN_MSG_TIMESTAMP timeStamp;
    } bF;
    uint32_t word[3];
    uint8_t byte[12];
} CAN_TX_MSGOBJ;

//! CAN RX Message Object

typedef union _CAN_RX_MSGOBJ {
    struct {
        CAN_MSGOBJ_ID id;
        CAN_RX_MSGOBJ_CTRL ctrl;
        CAN_MSG_TIMESTAMP timeStamp;
    } bF;
    uint32_t word[3];
    uint8_t byte[12];
} CAN_RX_MSGOBJ;

//! CAN TEF Message Object

typedef union _CAN_TEF_MSGOBJ {
    struct {
        CAN_MSGOBJ_ID id;
        CAN_TX_MSGOBJ_CTRL ctrl;
        CAN_MSG_TIMESTAMP timeStamp;
    } bF;
    uint32_t word[3];
    uint8_t byte[12];
} CAN_TEF_MSGOBJ;

//! CAN Filter Object ID

typedef struct _CAN_FILTEROBJ_ID {
    uint32_t SID            : 11;
    uint32_t EID            : 18;
    uint32_t SID11          : 1;
    uint32_t EXIDE          : 1;
    uint32_t unimplemented1 : 1;
} CAN_FILTEROBJ_ID;

//! CAN Mask Object ID

typedef struct _CAN_MASKOBJ_ID {
    uint32_t MSID           : 11;
    uint32_t MEID           : 18;
    uint32_t MSID11         : 1;
    uint32_t MIDE           : 1;
    uint32_t unimplemented1 : 1;
} CAN_MASKOBJ_ID;

//! CAN RX FIFO Status

typedef enum {
    CAN_RX_FIFO_EMPTY = 0,
    CAN_RX_FIFO_STATUS_MASK = 0x0F,
    CAN_RX_FIFO_NOT_EMPTY = 0x01,
    CAN_RX_FIFO_HALF_FULL = 0x02,
    CAN_RX_FIFO_FULL = 0x04,
    CAN_RX_FIFO_OVERFLOW = 0x08
} CAN_RX_FIFO_STATUS;

//! CAN TX FIFO Status

typedef enum {
    CAN_TX_FIFO_FULL = 0,
    CAN_TX_FIFO_STATUS_MASK = 0x1F7,
    CAN_TX_FIFO_NOT_FULL = 0x01,
    CAN_TX_FIFO_HALF_FULL = 0x02,
    CAN_TX_FIFO_EMPTY = 0x04,
    CAN_TX_FIFO_ATTEMPTS_EXHAUSTED = 0x10,
    CAN_TX_FIFO_ERROR = 0x20,
    CAN_TX_FIFO_ARBITRATION_LOST = 0x40,
    CAN_TX_FIFO_ABORTED = 0x80,
    CAN_TX_FIFO_TRANSMITTING = 0x100
} CAN_TX_FIFO_STATUS;

//! CAN TEF FIFO Status

typedef enum {
    CAN_TEF_FIFO_EMPTY = 0,
    CAN_TEF_FIFO_STATUS_MASK = 0x0F,
    CAN_TEF_FIFO_NOT_EMPTY = 0x01,
    CAN_TEF_FIFO_HALF_FULL = 0x02,
    CAN_TEF_FIFO_FULL = 0x04,
    CAN_TEF_FIFO_OVERFLOW = 0x08
} CAN_TEF_FIFO_STATUS;

//! CAN Module Event (Interrupts)

typedef enum {
    CAN_NO_EVENT = 0,
    CAN_ALL_EVENTS = 0xFF1F0000,
    CAN_TX_EVENT = 0x0001,
    CAN_RX_EVENT = 0x0002,
    CAN_TIME_BASE_COUNTER_EVENT = 0x0004,
    CAN_OPERATION_MODE_CHANGE_EVENT = 0x0008,
    CAN_TEF_EVENT = 0x0010,

    CAN_RAM_ECC_EVENT = 0x0100,
    CAN_SPI_CRC_EVENT = 0x0200,
    CAN_TX_ATTEMPTS_EVENT = 0x0400,
    CAN_RX_OVERFLOW_EVENT = 0x0800,
    CAN_SYSTEM_ERROR_EVENT = 0x1000,
    CAN_BUS_ERROR_EVENT = 0x2000,
    CAN_BUS_WAKEUP_EVENT = 0x4000,
    CAN_RX_INVALID_MESSAGE_EVENT = 0x8000
} CAN_MODULE_EVENT;

//! CAN TX FIFO Event (Interrupts)

typedef enum {
    CAN_TX_FIFO_NO_EVENT = 0,
    CAN_TX_FIFO_ALL_EVENTS = 0x17,
    CAN_TX_FIFO_NOT_FULL_EVENT = 0x01,
    CAN_TX_FIFO_HALF_FULL_EVENT = 0x02,
    CAN_TX_FIFO_EMPTY_EVENT = 0x04,
    CAN_TX_FIFO_ATTEMPTS_EXHAUSTED_EVENT = 0x10
} CAN_TX_FIFO_EVENT;

//! CAN RX FIFO Event (Interrupts)

typedef enum {
    CAN_RX_FIFO_NO_EVENT = 0,
    CAN_RX_FIFO_ALL_EVENTS = 0x0F,
    CAN_RX_FIFO_NOT_EMPTY_EVENT = 0x01,
    CAN_RX_FIFO_HALF_FULL_EVENT = 0x02,
    CAN_RX_FIFO_FULL_EVENT = 0x04,
    CAN_RX_FIFO_OVERFLOW_EVENT = 0x08
} CAN_RX_FIFO_EVENT;

//! CAN TEF FIFO Event (Interrupts)

typedef enum {
    CAN_TEF_FIFO_NO_EVENT = 0,
    CAN_TEF_FIFO_ALL_EVENTS = 0x0F,
    CAN_TEF_FIFO_NOT_EMPTY_EVENT = 0x01,
    CAN_TEF_FIFO_HALF_FULL_EVENT = 0x02,
    CAN_TEF_FIFO_FULL_EVENT = 0x04,
    CAN_TEF_FIFO_OVERFLOW_EVENT = 0x08
} CAN_TEF_FIFO_EVENT;

//! CAN Bit Time Setup: Arbitration/Data Bit Phase

/* not apply to AVR platform */
// typedef enum {
static const uint32_t CAN_125K_500K = (4UL << 24) | (125000UL);
static const uint32_t CAN_250K_500K = (2UL << 24) | (250000UL);
static const uint32_t CAN_250K_750K = (3UL << 24) | (250000UL);
static const uint32_t CAN_250K_1M = (4UL << 24) | (250000UL);
static const uint32_t CAN_250K_1M5 = (6UL << 24) | (250000UL);
static const uint32_t CAN_250K_2M = (8UL << 24) | (250000UL);
static const uint32_t CAN_250K_3M = (12UL << 24) | (250000UL);
static const uint32_t CAN_250K_4M = (16UL << 24) | (250000UL);
static const uint32_t CAN_500K_1M = (2UL << 24) | (500000UL);
static const uint32_t CAN_500K_2M = (4UL << 24) | (500000UL);
static const uint32_t CAN_500K_3M = (6UL << 24) | (500000UL);
static const uint32_t CAN_500K_4M = (8UL << 24) | (500000UL);
static const uint32_t CAN_500K_5M = (10UL << 24) | (500000UL);
static const uint32_t CAN_500K_6M5 = (13UL << 24) | (500000UL);
static const uint32_t CAN_500K_8M = (16UL << 24) | (500000UL);
static const uint32_t CAN_500K_10M = (20UL << 24) | (500000UL);
static const uint32_t CAN_1000K_4M = (4UL << 24) | (1000000UL);
static const uint32_t CAN_1000K_8M = (8UL << 24) | (1000000UL);
//} MCP2518FD_BITTIME_SETUP;

//! Secondary Sample Point Mode

typedef enum {
    CAN_SSP_MODE_OFF,
    CAN_SSP_MODE_MANUAL,
    CAN_SSP_MODE_AUTO
} CAN_SSP_MODE;

//! CAN Error State

typedef enum {
    CAN_ERROR_FREE_STATE = 0,
    CAN_ERROR_ALL = 0x3F,
    CAN_TX_RX_WARNING_STATE = 0x01,
    CAN_RX_WARNING_STATE = 0x02,
    CAN_TX_WARNING_STATE = 0x04,
    CAN_RX_BUS_PASSIVE_STATE = 0x08,
    CAN_TX_BUS_PASSIVE_STATE = 0x10,
    CAN_TX_BUS_OFF_STATE = 0x20
} CAN_ERROR_STATE;

//! CAN Time Stamp Mode Select

typedef enum {
    CAN_TS_SOF = 0x00,
    CAN_TS_EOF = 0x01,
    CAN_TS_RES = 0x02
} CAN_TS_MODE;

//! CAN ECC EVENT

typedef enum {
    CAN_ECC_NO_EVENT = 0x00,
    CAN_ECC_ALL_EVENTS = 0x06,
    CAN_ECC_SEC_EVENT = 0x02,
    CAN_ECC_DED_EVENT = 0x04
} CAN_ECC_EVENT;

//! CAN CRC EVENT

typedef enum {
    CAN_CRC_NO_EVENT = 0x00,
    CAN_CRC_ALL_EVENTS = 0x03,
    CAN_CRC_CRCERR_EVENT = 0x01,
    CAN_CRC_FORMERR_EVENT = 0x02
} CAN_CRC_EVENT;

//! GPIO Pin Position

typedef enum {
    GPIO_PIN_0,
    GPIO_PIN_1
} GPIO_PIN_POS;

//! GPIO Pin Modes

typedef enum {
    GPIO_MODE_INT,
    GPIO_MODE_GPIO
} GPIO_PIN_MODE;

//! GPIO Pin Directions

typedef enum {
    GPIO_OUTPUT,
    GPIO_INPUT
} GPIO_PIN_DIRECTION;

//! GPIO Open Drain Mode

typedef enum {
    GPIO_PUSH_PULL,
    GPIO_OPEN_DRAIN
} GPIO_OPEN_DRAIN_MODE;

//! GPIO Pin State

typedef enum {
    GPIO_LOW,
    GPIO_HIGH
} GPIO_PIN_STATE;

//! Clock Output Mode

typedef enum {
    GPIO_CLKO_CLOCK,
    GPIO_CLKO_SOF
} GPIO_CLKO_MODE;

//! CAN Bus Diagnostic flags

typedef struct _CAN_BUS_DIAG_FLAGS {
    uint32_t NBIT0_ERR      : 1;
    uint32_t NBIT1_ERR      : 1;
    uint32_t NACK_ERR       : 1;
    uint32_t NFORM_ERR      : 1;
    uint32_t NSTUFF_ERR     : 1;
    uint32_t NCRC_ERR       : 1;
    uint32_t unimplemented1 : 1;
    uint32_t TXBO_ERR       : 1;
    uint32_t DBIT0_ERR      : 1;
    uint32_t DBIT1_ERR      : 1;
    uint32_t unimplemented2 : 1;
    uint32_t DFORM_ERR      : 1;
    uint32_t DSTUFF_ERR     : 1;
    uint32_t DCRC_ERR       : 1;
    uint32_t ESI            : 1;
    uint32_t DLC_MISMATCH   : 1;
} CAN_BUS_DIAG_FLAGS;

//! CAN Bus Diagnostic Error Counts

typedef struct _CAN_BUS_ERROR_COUNT {
    uint8_t NREC;
    uint8_t NTEC;
    uint8_t DREC;
    uint8_t DTEC;
} CAN_BUS_ERROR_COUNT;

//! CAN BUS DIAGNOSTICS

typedef union _CAN_BUS_DIAGNOSTIC {
    struct {
        CAN_BUS_ERROR_COUNT errorCount;
        uint16_t errorFreeMsgCount;
        CAN_BUS_DIAG_FLAGS flag;
    } bF;
    uint32_t word[3];
    uint8_t byte[12];
} CAN_BUS_DIAGNOSTIC;

//! TXREQ Channel Bits
// Multiple channels can be or'ed together

typedef enum {
    CAN_TXREQ_CH0 = 0x00000001,
    CAN_TXREQ_CH1 = 0x00000002,
    CAN_TXREQ_CH2 = 0x00000004,
    CAN_TXREQ_CH3 = 0x00000008,
    CAN_TXREQ_CH4 = 0x00000010,
    CAN_TXREQ_CH5 = 0x00000020,
    CAN_TXREQ_CH6 = 0x00000040,
    CAN_TXREQ_CH7 = 0x00000080,

    CAN_TXREQ_CH8 = 0x00000100,
    CAN_TXREQ_CH9 = 0x00000200,
    CAN_TXREQ_CH10 = 0x00000400,
    CAN_TXREQ_CH11 = 0x00000800,
    CAN_TXREQ_CH12 = 0x00001000,
    CAN_TXREQ_CH13 = 0x00002000,
    CAN_TXREQ_CH14 = 0x00004000,
    CAN_TXREQ_CH15 = 0x00008000,

    CAN_TXREQ_CH16 = 0x00010000,
    CAN_TXREQ_CH17 = 0x00020000,
    CAN_TXREQ_CH18 = 0x00040000,
    CAN_TXREQ_CH19 = 0x00080000,
    CAN_TXREQ_CH20 = 0x00100000,
    CAN_TXREQ_CH21 = 0x00200000,
    CAN_TXREQ_CH22 = 0x00400000,
    CAN_TXREQ_CH23 = 0x00800000,

    CAN_TXREQ_CH24 = 0x01000000,
    CAN_TXREQ_CH25 = 0x02000000,
    CAN_TXREQ_CH26 = 0x04000000,
    CAN_TXREQ_CH27 = 0x08000000,
    CAN_TXREQ_CH28 = 0x10000000,
    CAN_TXREQ_CH29 = 0x20000000,
    CAN_TXREQ_CH30 = 0x40000000,
    CAN_TXREQ_CH31 = 0x80000000
} CAN_TXREQ_CHANNEL;

//! Oscillator Control

typedef struct _CAN_OSC_CTRL {
    uint32_t PllEnable    : 1;
    uint32_t OscDisable   : 1;
    uint32_t SclkDivide   : 1;
    uint32_t ClkOutDivide : 2;
#ifndef MCP2517FD
    uint32_t LowPowerModeEnable : 1;
#endif
} CAN_OSC_CTRL;

//! Oscillator Status

typedef struct _CAN_OSC_STATUS {
    uint32_t PllReady  : 1;
    uint32_t OscReady  : 1;
    uint32_t SclkReady : 1;
} CAN_OSC_STATUS;

//! ICODE

typedef enum {
    CAN_ICODE_FIFO_CH0,
    CAN_ICODE_FIFO_CH1,
    CAN_ICODE_FIFO_CH2,
    CAN_ICODE_FIFO_CH3,
    CAN_ICODE_FIFO_CH4,
    CAN_ICODE_FIFO_CH5,
    CAN_ICODE_FIFO_CH6,
    CAN_ICODE_FIFO_CH7,
    CAN_ICODE_FIFO_CH8,
    CAN_ICODE_FIFO_CH9,
    CAN_ICODE_FIFO_CH10,
    CAN_ICODE_FIFO_CH11,
    CAN_ICODE_FIFO_CH12,
    CAN_ICODE_FIFO_CH13,
    CAN_ICODE_FIFO_CH14,
    CAN_ICODE_FIFO_CH15,
    CAN_ICODE_FIFO_CH16,
    CAN_ICODE_FIFO_CH17,
    CAN_ICODE_FIFO_CH18,
    CAN_ICODE_FIFO_CH19,
    CAN_ICODE_FIFO_CH20,
    CAN_ICODE_FIFO_CH21,
    CAN_ICODE_FIFO_CH22,
    CAN_ICODE_FIFO_CH23,
    CAN_ICODE_FIFO_CH24,
    CAN_ICODE_FIFO_CH25,
    CAN_ICODE_FIFO_CH26,
    CAN_ICODE_FIFO_CH27,
    CAN_ICODE_FIFO_CH28,
    CAN_ICODE_FIFO_CH29,
    CAN_ICODE_FIFO_CH30,
    CAN_ICODE_FIFO_CH31,
    CAN_ICODE_TOTAL_CHANNELS,
    CAN_ICODE_NO_INT = 0x40,
    CAN_ICODE_CERRIF,
    CAN_ICODE_WAKIF,
    CAN_ICODE_RXOVIF,
    CAN_ICODE_ADDRERR_SERRIF,
    CAN_ICODE_MABOV_SERRIF,
    CAN_ICODE_TBCIF,
    CAN_ICODE_MODIF,
    CAN_ICODE_IVMIF,
    CAN_ICODE_TEFIF,
    CAN_ICODE_TXATIF,
    CAN_ICODE_RESERVED
} CAN_ICODE;

//! RXCODE

typedef enum {
    CAN_RXCODE_FIFO_CH1 = 1,
    CAN_RXCODE_FIFO_CH2,
    CAN_RXCODE_FIFO_CH3,
    CAN_RXCODE_FIFO_CH4,
    CAN_RXCODE_FIFO_CH5,
    CAN_RXCODE_FIFO_CH6,
    CAN_RXCODE_FIFO_CH7,
    CAN_RXCODE_FIFO_CH8,
    CAN_RXCODE_FIFO_CH9,
    CAN_RXCODE_FIFO_CH10,
    CAN_RXCODE_FIFO_CH11,
    CAN_RXCODE_FIFO_CH12,
    CAN_RXCODE_FIFO_CH13,
    CAN_RXCODE_FIFO_CH14,
    CAN_RXCODE_FIFO_CH15,
    CAN_RXCODE_FIFO_CH16,
    CAN_RXCODE_FIFO_CH17,
    CAN_RXCODE_FIFO_CH18,
    CAN_RXCODE_FIFO_CH19,
    CAN_RXCODE_FIFO_CH20,
    CAN_RXCODE_FIFO_CH21,
    CAN_RXCODE_FIFO_CH22,
    CAN_RXCODE_FIFO_CH23,
    CAN_RXCODE_FIFO_CH24,
    CAN_RXCODE_FIFO_CH25,
    CAN_RXCODE_FIFO_CH26,
    CAN_RXCODE_FIFO_CH27,
    CAN_RXCODE_FIFO_CH28,
    CAN_RXCODE_FIFO_CH29,
    CAN_RXCODE_FIFO_CH30,
    CAN_RXCODE_FIFO_CH31,
    CAN_RXCODE_TOTAL_CHANNELS,
    CAN_RXCODE_NO_INT = 0x40,
    CAN_RXCODE_RESERVED
} CAN_RXCODE;

//! TXCODE

typedef enum {
    CAN_TXCODE_FIFO_CH0,
    CAN_TXCODE_FIFO_CH1,
    CAN_TXCODE_FIFO_CH2,
    CAN_TXCODE_FIFO_CH3,
    CAN_TXCODE_FIFO_CH4,
    CAN_TXCODE_FIFO_CH5,
    CAN_TXCODE_FIFO_CH6,
    CAN_TXCODE_FIFO_CH7,
    CAN_TXCODE_FIFO_CH8,
    CAN_TXCODE_FIFO_CH9,
    CAN_TXCODE_FIFO_CH10,
    CAN_TXCODE_FIFO_CH11,
    CAN_TXCODE_FIFO_CH12,
    CAN_TXCODE_FIFO_CH13,
    CAN_TXCODE_FIFO_CH14,
    CAN_TXCODE_FIFO_CH15,
    CAN_TXCODE_FIFO_CH16,
    CAN_TXCODE_FIFO_CH17,
    CAN_TXCODE_FIFO_CH18,
    CAN_TXCODE_FIFO_CH19,
    CAN_TXCODE_FIFO_CH20,
    CAN_TXCODE_FIFO_CH21,
    CAN_TXCODE_FIFO_CH22,
    CAN_TXCODE_FIFO_CH23,
    CAN_TXCODE_FIFO_CH24,
    CAN_TXCODE_FIFO_CH25,
    CAN_TXCODE_FIFO_CH26,
    CAN_TXCODE_FIFO_CH27,
    CAN_TXCODE_FIFO_CH28,
    CAN_TXCODE_FIFO_CH29,
    CAN_TXCODE_FIFO_CH30,
    CAN_TXCODE_FIFO_CH31,
    CAN_TXCODE_TOTAL_CHANNELS,
    CAN_TXCODE_NO_INT = 0x40,
    CAN_TXCODE_RESERVED
} CAN_TXCODE;

//! System Clock Selection

typedef enum {
    CAN_SYSCLK_40M = MCP2518FD_40MHz,
    CAN_SYSCLK_20M = MCP2518FD_20MHz,
    CAN_SYSCLK_10M = MCP2518FD_10MHz,
} CAN_SYSCLK_SPEED;

//! CLKO Divide

typedef enum {
    OSC_CLKO_DIV1,
    OSC_CLKO_DIV2,
    OSC_CLKO_DIV4,
    OSC_CLKO_DIV10
} OSC_CLKO_DIVIDE;

// *****************************************************************************
//! General 32-bit Register

typedef union _REG_t {
    uint8_t byte[4];
    uint32_t word;
} REG_t;

// *****************************************************************************
// *****************************************************************************
/* CAN FD Controller */

// *****************************************************************************
//! CAN Control Register

typedef union _REG_CiCON {
    struct {
        uint32_t DNetFilterCount               : 5;
        uint32_t IsoCrcEnable                  : 1;
        uint32_t ProtocolExceptionEventDisable : 1;
        uint32_t unimplemented1                : 1;
        uint32_t WakeUpFilterEnable            : 1;
        uint32_t WakeUpFilterTime              : 2;
        uint32_t unimplemented2                : 1;
        uint32_t BitRateSwitchDisable          : 1;
        uint32_t unimplemented3                : 3;
        uint32_t RestrictReTxAttempts          : 1;
        uint32_t EsiInGatewayMode              : 1;
        uint32_t SystemErrorToListenOnly       : 1;
        uint32_t StoreInTEF                    : 1;
        uint32_t TXQEnable                     : 1;
        uint32_t OpMode                        : 3;
        uint32_t RequestOpMode                 : 3;
        uint32_t AbortAllTx                    : 1;
        uint32_t TxBandWidthSharing            : 4;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiCON;

// *****************************************************************************
//! Nominal Bit Time Configuration Register

typedef union _REG_CiNBTCFG {
    struct {
        uint32_t SJW            : 7;
        uint32_t unimplemented1 : 1;
        uint32_t TSEG2          : 7;
        uint32_t unimplemented2 : 1;
        uint32_t TSEG1          : 8;
        uint32_t BRP            : 8;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiNBTCFG;

// *****************************************************************************
//! Data Bit Time Configuration Register

typedef union _REG_CiDBTCFG {
    struct {
        uint32_t SJW            : 4;
        uint32_t unimplemented1 : 4;
        uint32_t TSEG2          : 4;
        uint32_t unimplemented2 : 4;
        uint32_t TSEG1          : 5;
        uint32_t unimplemented3 : 3;
        uint32_t BRP            : 8;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiDBTCFG;

// *****************************************************************************
//! Transmitter Delay Compensation Register

typedef union _REG_CiTDC {
    struct {
        uint32_t TDCValue         : 6;
        uint32_t unimplemented1   : 2;
        uint32_t TDCOffset        : 7;
        uint32_t unimplemented2   : 1;
        uint32_t TDCMode          : 2;
        uint32_t unimplemented3   : 6;
        uint32_t SID11Enable      : 1;
        uint32_t EdgeFilterEnable : 1;
        uint32_t unimplemented4   : 6;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTDC;

// *****************************************************************************
//! Time Stamp Configuration Register

typedef union _REG_CiTSCON {
    struct {
        uint32_t TBCPrescaler   : 10;
        uint32_t unimplemented1 : 6;
        uint32_t TBCEnable      : 1;
        uint32_t TimeStampEOF   : 1;
        uint32_t unimplemented2 : 14;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTSCON;

// *****************************************************************************
//! Interrupt Vector Register

typedef union _REG_CiVEC {
    struct {
        uint32_t ICODE          : 7;
        uint32_t unimplemented1 : 1;
        uint32_t FilterHit      : 5;
        uint32_t unimplemented2 : 3;
        uint32_t TXCODE         : 7;
        uint32_t unimplemented3 : 1;
        uint32_t RXCODE         : 7;
        uint32_t unimplemented4 : 1;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiVEC;

// *****************************************************************************
//! Interrupt Flags

typedef struct _CAN_INT_FLAGS {
    uint32_t TXIF           : 1;
    uint32_t RXIF           : 1;
    uint32_t TBCIF          : 1;
    uint32_t MODIF          : 1;
    uint32_t TEFIF          : 1;
    uint32_t unimplemented1 : 3;

    uint32_t ECCIF          : 1;
    uint32_t SPICRCIF       : 1;
    uint32_t TXATIF         : 1;
    uint32_t RXOVIF         : 1;
    uint32_t SERRIF         : 1;
    uint32_t CERRIF         : 1;
    uint32_t WAKIF          : 1;
    uint32_t IVMIF          : 1;
} CAN_INT_FLAGS;

// *****************************************************************************
//! Interrupt Enables

typedef struct _CAN_INT_ENABLES {
    uint32_t TXIE           : 1;
    uint32_t RXIE           : 1;
    uint32_t TBCIE          : 1;
    uint32_t MODIE          : 1;
    uint32_t TEFIE          : 1;
    uint32_t unimplemented2 : 3;

    uint32_t ECCIE          : 1;
    uint32_t SPICRCIE       : 1;
    uint32_t TXATIE         : 1;
    uint32_t RXOVIE         : 1;
    uint32_t SERRIE         : 1;
    uint32_t CERRIE         : 1;
    uint32_t WAKIE          : 1;
    uint32_t IVMIE          : 1;
} CAN_INT_ENABLES;

// *****************************************************************************
//! Interrupt Register

typedef union _REG_CiINT {
    struct {
        CAN_INT_FLAGS IF;
        CAN_INT_ENABLES IE;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiINT;

// *****************************************************************************
//! Interrupt Flag Register

typedef union _REG_CiINTFLAG {
    CAN_INT_FLAGS IF;
    uint16_t word;
    uint8_t byte[2];
} REG_CiINTFLAG;

// *****************************************************************************
//! Interrupt Enable Register

typedef union _REG_CiINTENABLE {
    CAN_INT_ENABLES IE;
    uint32_t word;
    uint8_t byte[4];
} REG_CiINTENABLE;

// *****************************************************************************
//! Transmit/Receive Error Count Register

typedef union _REG_CiTREC {
    struct {
        uint32_t RxErrorCount        : 8;
        uint32_t TxErrorCount        : 8;
        uint32_t ErrorStateWarning   : 1;
        uint32_t RxErrorStateWarning : 1;
        uint32_t TxErrorStateWarning : 1;
        uint32_t RxErrorStatePassive : 1;
        uint32_t TxErrorStatePassive : 1;
        uint32_t TxErrorStateBusOff  : 1;
        uint32_t unimplemented1      : 10;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTREC;

// *****************************************************************************
//! Diagnostic Register 0

typedef union _REG_CiBDIAG0 {
    struct {
        uint32_t NRxErrorCount : 8;
        uint32_t NTxErrorCount : 8;
        uint32_t DRxErrorCount : 8;
        uint32_t DTxErrorCount : 8;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiBDIAG0;

// *****************************************************************************
//! Diagnostic Register 1

typedef union _REG_CiBDIAG1 {
    struct {
        uint32_t ErrorFreeMsgCount : 16;

        uint32_t NBit0Error        : 1;
        uint32_t NBit1Error        : 1;
        uint32_t NAckError         : 1;
        uint32_t NFormError        : 1;
        uint32_t NStuffError       : 1;
        uint32_t NCRCError         : 1;
        uint32_t unimplemented1    : 1;
        uint32_t TXBOError         : 1;
        uint32_t DBit0Error        : 1;
        uint32_t DBit1Error        : 1;
        uint32_t DAckError         : 1;
        uint32_t DFormError        : 1;
        uint32_t DStuffError       : 1;
        uint32_t DCRCError         : 1;
        uint32_t ESI               : 1;
        uint32_t unimplemented2    : 1;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiBDIAG1;

// *****************************************************************************
//! Transmit Event FIFO Control Register

typedef union _REG_CiTEFCON {
    struct {
        uint32_t TEFNEIE         : 1;
        uint32_t TEFHFIE         : 1;
        uint32_t TEFFULIE        : 1;
        uint32_t TEFOVIE         : 1;
        uint32_t unimplemented1  : 1;
        uint32_t TimeStampEnable : 1;
        uint32_t unimplemented2  : 2;
        uint32_t UINC            : 1;
        uint32_t unimplemented3  : 1;
        uint32_t FRESET          : 1;
        uint32_t unimplemented4  : 13;
        uint32_t FifoSize        : 5;
        uint32_t unimplemented5  : 3;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTEFCON;

// *****************************************************************************
//! Transmit Event FIFO Status Register

typedef union _REG_CiTEFSTA {
    struct {
        uint32_t TEFNotEmptyIF  : 1;
        uint32_t TEFHalfFullIF  : 1;
        uint32_t TEFFullIF      : 1;
        uint32_t TEFOVIF        : 1;
        uint32_t unimplemented1 : 28;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTEFSTA;

// *****************************************************************************
//! Transmit Queue Control Register

typedef union _REG_CiTXQCON {
    struct {
        uint32_t TxNotFullIE    : 1;
        uint32_t unimplemented1 : 1;
        uint32_t TxEmptyIE      : 1;
        uint32_t unimplemented2 : 1;
        uint32_t TxAttemptIE    : 1;
        uint32_t unimplemented3 : 2;
        uint32_t TxEnable       : 1;
        uint32_t UINC           : 1;
        uint32_t TxRequest      : 1;
        uint32_t FRESET         : 1;
        uint32_t unimplemented4 : 5;
        uint32_t TxPriority     : 5;
        uint32_t TxAttempts     : 2;
        uint32_t unimplemented5 : 1;
        uint32_t FifoSize       : 5;
        uint32_t PayLoadSize    : 3;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTXQCON;

// *****************************************************************************
//! Transmit Queue Status Register

typedef union _REG_CiTXQSTA {
    struct {
        uint32_t TxNotFullIF       : 1;
        uint32_t unimplemented1    : 1;
        uint32_t TxEmptyIF         : 1;
        uint32_t unimplemented2    : 1;
        uint32_t TxAttemptIF       : 1;
        uint32_t TxError           : 1;
        uint32_t TxLostArbitration : 1;
        uint32_t TxAborted         : 1;
        uint32_t FifoIndex         : 5;
        uint32_t unimplemented3    : 19;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTXQSTA;

// *****************************************************************************
//! FIFO Control Register

typedef union _REG_CiFIFOCON {
    // Receive FIFO

    struct {
        uint32_t RxNotEmptyIE      : 1;
        uint32_t RxHalfFullIE      : 1;
        uint32_t RxFullIE          : 1;
        uint32_t RxOverFlowIE      : 1;
        uint32_t unimplemented1    : 1;
        uint32_t RxTimeStampEnable : 1;
        uint32_t unimplemented2    : 1;
        uint32_t TxEnable          : 1;
        uint32_t UINC              : 1;
        uint32_t unimplemented3    : 1;
        uint32_t FRESET            : 1;
        uint32_t unimplemented4    : 13;
        uint32_t FifoSize          : 5;
        uint32_t PayLoadSize       : 3;
    } rxBF;

    // Transmit FIFO

    struct {
        uint32_t TxNotFullIE    : 1;
        uint32_t TxHalfFullIE   : 1;
        uint32_t TxEmptyIE      : 1;
        uint32_t unimplemented1 : 1;
        uint32_t TxAttemptIE    : 1;
        uint32_t unimplemented2 : 1;
        uint32_t RTREnable      : 1;
        uint32_t TxEnable       : 1;
        uint32_t UINC           : 1;
        uint32_t TxRequest      : 1;
        uint32_t FRESET         : 1;
        uint32_t unimplemented3 : 5;
        uint32_t TxPriority     : 5;
        uint32_t TxAttempts     : 2;
        uint32_t unimplemented4 : 1;
        uint32_t FifoSize       : 5;
        uint32_t PayLoadSize    : 3;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOCON;

// *****************************************************************************
//! FIFO Status Register

typedef union _REG_CiFIFOSTA {
    // Receive FIFO

    struct {
        uint32_t RxNotEmptyIF   : 1;
        uint32_t RxHalfFullIF   : 1;
        uint32_t RxFullIF       : 1;
        uint32_t RxOverFlowIF   : 1;
        uint32_t unimplemented1 : 4;
        uint32_t FifoIndex      : 5;
        uint32_t unimplemented2 : 19;
    } rxBF;

    // Transmit FIFO

    struct {
        uint32_t TxNotFullIF       : 1;
        uint32_t TxHalfFullIF      : 1;
        uint32_t TxEmptyIF         : 1;
        uint32_t unimplemented1    : 1;
        uint32_t TxAttemptIF       : 1;
        uint32_t TxError           : 1;
        uint32_t TxLostArbitration : 1;
        uint32_t TxAborted         : 1;
        uint32_t FifoIndex         : 5;
        uint32_t unimplemented2    : 19;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOSTA;

// *****************************************************************************
//! FIFO User Address Register

typedef union _REG_CiFIFOUA {
    struct {
        uint32_t UserAddress    : 12;
        uint32_t unimplemented1 : 20;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOUA;

// *****************************************************************************
//! Filter Control Register

typedef union _REG_CiFLTCON_BYTE {
    struct {
        uint32_t BufferPointer  : 5;
        uint32_t unimplemented1 : 2;
        uint32_t Enable         : 1;
    } bF;
    uint8_t byte;
} REG_CiFLTCON_BYTE;

// *****************************************************************************
//! Filter Object Register

typedef union _REG_CiFLTOBJ {
    CAN_FILTEROBJ_ID bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFLTOBJ;

// *****************************************************************************
//! Mask Object Register

typedef union _REG_CiMASK {
    CAN_MASKOBJ_ID bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiMASK;

// *****************************************************************************
// *****************************************************************************
/* MCP25xxFD Specific */

// *****************************************************************************
//! Oscillator Control Register

typedef union _REG_OSC {
    struct {
        uint32_t PllEnable      : 1;
        uint32_t unimplemented1 : 1;
        uint32_t OscDisable     : 1;
#ifdef MCP2517FD
        uint32_t unimplemented2 : 1;
#else
        uint32_t LowPowerModeEnable : 1;
#endif
        uint32_t SCLKDIV        : 1;
        uint32_t CLKODIV        : 2;
        uint32_t unimplemented3 : 1;
        uint32_t PllReady       : 1;
        uint32_t unimplemented4 : 1;
        uint32_t OscReady       : 1;
        uint32_t unimplemented5 : 1;
        uint32_t SclkReady      : 1;
        uint32_t unimplemented6 : 19;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_OSC;

// *****************************************************************************
//! I/O Control Register

typedef union _REG_IOCON {
    struct {
        uint32_t TRIS0                 : 1;
        uint32_t TRIS1                 : 1;
        uint32_t unimplemented1        : 2;
        uint32_t ClearAutoSleepOnMatch : 1;
        uint32_t AutoSleepEnable       : 1;
        uint32_t XcrSTBYEnable         : 1;
        uint32_t unimplemented2        : 1;
        uint32_t LAT0                  : 1;
        uint32_t LAT1                  : 1;
        uint32_t unimplemented3        : 5;
        uint32_t HVDETSEL              : 1;
        uint32_t GPIO0                 : 1;
        uint32_t GPIO1                 : 1;
        uint32_t unimplemented4        : 6;
        uint32_t PinMode0              : 1;
        uint32_t PinMode1              : 1;
        uint32_t unimplemented5        : 2;
        uint32_t TXCANOpenDrain        : 1;
        uint32_t SOFOutputEnable       : 1;
        uint32_t INTPinOpenDrain       : 1;
        uint32_t unimplemented6        : 1;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_IOCON;

// *****************************************************************************
//! CRC Regsiter

typedef union _REG_CRC {
    struct {
        uint32_t CRC_MCP        : 16;
        uint32_t CRCERRIF       : 1;
        uint32_t FERRIF         : 1;
        uint32_t unimplemented1 : 6;
        uint32_t CRCERRIE       : 1;
        uint32_t FERRIE         : 1;
        uint32_t unimplemented2 : 6;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CRC;

// *****************************************************************************
//! ECC Control Register

typedef union _REG_ECCCON {
    struct {
        uint32_t EccEn          : 1;
        uint32_t SECIE          : 1;
        uint32_t DEDIE          : 1;
        uint32_t unimplemented1 : 5;
        uint32_t Parity         : 7;
        uint32_t unimplemented2 : 17;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_ECCCON;

// *****************************************************************************
//! ECC Status Register

typedef union _REG_ECCSTA {
    struct {
        uint32_t unimplemented1 : 1;
        uint32_t SECIF          : 1;
        uint32_t DEDIF          : 1;
        uint32_t unimplemented2 : 13;
        uint32_t ErrorAddress   : 12;
        uint32_t unimplemented3 : 4;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_ECCSTA;

// *****************************************************************************
//! DEVID Register

#ifndef MCP2517FD
typedef union _REG_DEVID {
    struct {
        uint32_t REV           : 4;
        uint32_t DEV           : 4;
        uint32_t unimplemented : 24;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_DEVID;
#endif

// *****************************************************************************
// *****************************************************************************
/* Register Reset Values */

// *****************************************************************************
/* CAN FD Controller */

// Control Register Reset Values up to FIFOs
static const uint32_t canControlResetValues[] = {
    /* Address 0x000 to 0x00C */
    0x04980760,
    0x003E0F0F,
    0x000E0303,
    0x00021000,
    /* Address 0x010 to 0x01C */
    0x00000000,
    0x00000000,
    0x40400040,
    0x00000000,
    /* Address 0x020 to 0x02C */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    /* Address 0x030 to 0x03C */
    0x00000000,
    0x00200000,
    0x00000000,
    0x00000000,
    /* Address 0x040 to 0x04C */
    0x00000400,
    0x00000000,
    0x00000000,
    0x00000000};

// FIFO Register Reset Values
static const uint32_t canFifoResetValues[] = {0x00600400, 0x00000000, 0x00000000};

// Filter Control Register Reset Values
static const uint32_t canFilterControlResetValue = 0x00000000;

// Filter and Mask Object Reset Values
static const uint32_t canFilterObjectResetValues[] = {0x00000000, 0x00000000};

// *****************************************************************************
/* MCP25xxFD */

#if defined(MCP2517FD) || defined(MCP2518FD)
static const uint32_t mcp25xxfdControlResetValues[] =
    {0x00000460, 0x00000003, 0x00000000, 0x00000000, 0x00000000};
#endif

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
#endif // _DRV_CANFDSPI_REGISTER_H
