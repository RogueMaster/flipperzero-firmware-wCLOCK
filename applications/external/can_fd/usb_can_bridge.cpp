/** @file usb_can_bridge.cpp
 * @brief @ref MODEL component main file. It mainly send SLCAN frames received on USB virtual com port to CAN bus and vice-versa. 
 * @ingroup MODEL
*/
/* ----------------------------------------------------------- INCLUSIONS--------------------------------------------------------------- */
#include "usb_can_bridge.hpp"
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <toolbox/api_lock.h>

#include <furi_hal_usb_cdc.h>
#include "usb_can_app_i.hpp"

/* ----------------------------------------------------------- MACROS--------------------------------------------------------------- */

/** @brief wait USB CDC driver is available : tx_mutex is used to protect access to shared flag @ref cdc_busy which is updated during interruption.
 * @details this macro will enter a loop which:
 * -# acquires tx_mutex to ensure it has exclusive access to @ref cdc_busy.
 * -# check if @ ref cdc_busy is clear:
 *     - If it is the case it set cdc_busy and exit loop (end of the the macro).
 *     - If it is not the case (a VCP transaction is ongoing) it release mutex before trying to reentering the loop.
*/
#define WAIT_CDC()                                                                          \
    while(true) {                                                                           \
        furi_check(furi_mutex_acquire(usb_can->tx_mutex, FuriWaitForever) == FuriStatusOk); \
        if(!cdc_busy) {                                                                     \
            cdc_busy = true;                                                                \
            furi_check(furi_mutex_release(usb_can->tx_mutex) == FuriStatusOk);              \
            break;                                                                          \
        }                                                                                   \
        furi_check(furi_mutex_release(usb_can->tx_mutex) == FuriStatusOk);                  \
    }
/** @brief Macro used when SLCAN command length is supperior than expected. It will call @ref usb_can_bridge_error with string @ref invalid_length*/
#define M_USB_CAN_ERROR_INVALID_LENGTH() \
    usb_can_bridge_error(usb_can, invalid_length, data[0], len)
/** @brief  Macro used when SLCAN command first character is erroneous. It is used in @ref usb_can_tx_thread. It will call @ref usb_can_bridge_error with string @ref invalid_command**/
#define M_USB_CAN_ERROR_INVALID_COMMAND() \
    usb_can_bridge_error(usb_can, invalid_command, data[0], data[0])

/* ----------------------------------------------------------- DEFINES (CONSTANTS)--------------------------------------------------------------- */
/** @brief  Thread flags definitions. They are set by interruptions callbacks to signal threads ( @ref usb_can_tx_thread and @ref usb_can_worker) an event is to be processed. */
typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),

    WorkerEvtTxStop = (1 << 2),
    WorkerEvtCdcRx = (1 << 3),

    WorkerEvtCfgChange = (1 << 4),
    WorkerEvtTestTx = (1 << 5)
} WorkerEvtFlags;

/** @brief used as condition to exit waiting routine for @ref usb_can_worker() */
#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone | WorkerEvtCfgChange)
/** @brief used as condition to exit waiting routine for @ref usb_can_tx_thread()**/
#define WORKER_ALL_TX_EVENTS (WorkerEvtTxStop | WorkerEvtCdcRx)
/** @brief used to allocate space on local USB CDC Rx buffer*/
#define USB_CAN_RX_BUF_SIZE (USB_CDC_PKT_LEN * 5)
/** @brief SLCAN command max length */
#define USB_CAN_BRIDGE_MAX_INPUT_LENGTH 28
/** @brief CAN test message length "CANLIVE"+termination character */
#define TEST_MESSAGE_LENGTH 8
/* ----------------------------------------------------------- LOCAL FUNCTIONS DECLARATIONS--------------------------------------------------------------- */
static void vcp_on_cdc_tx_complete(void* context);
static void vcp_on_cdc_rx(void* context);
static void vcp_state_callback(void* context, uint8_t state);
static void vcp_on_cdc_control_line(void* context, uint8_t state);
static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config);
static void usb_can_bridge_error(UsbCanBridge* usb_can, const char* err_msg_format, ...);
static void usb_can_bridge_configure(UsbCanBridge* usb_can, char* data, uint32_t len);
static void
    usb_can_bridge_send(UsbCanBridge* usb_can, char* data, uint32_t len, bool extended, bool rtr);
static bool _usb_uart_bridge_get_next_command(
    char** next_command,
    uint8_t* cmd_length,
    char* cdc_data,
    uint8_t cdc_frame_len);

static int32_t usb_can_tx_thread(void* context);
static void usb_can_bridge_clear_led(void* context);

/* ----------------------------------------------------------- CONSTANTS--------------------------------------------------------------- */
/** @brief Serma banner to be displayed on USB virtual com port (VCP) via @ref usb_can_print_banner() when entering in the APP operationnal mode.*/
const char serma_banner[] =
    "\r\n"
    "   ___________  __  ______     _______  ___    ________   _  __  _______ \r\n"
    "  / __/ __/ _ \\/  |/  / _ |   / __/ _ \\/ _ \\  / ___/ _ | / |/ / / __/ _ \\ \r\n"
    " _\\ \\/ _// , _/ /|_/ / __ |  / _// ___/ // / / /__/ __ |/    / / _// // /\r\n"
    "/___/___/_/|_/_/  /_/_/ |_| /_/ /_/   \\___/  \\___/_/ |_/_/|_/ /_/ /____/ \r\n"
    "Serma Safety and Security (S3) - 2024 \r\n";

/**  @brief message used to signal to user that SLCAN command length is invalid*/
const char invalid_length[] = "[err]: invalid command length (command %c) :length = %d\r\n";

/**  @brief message used when user sends "d\\r\\n" while in USB-CAN-BRIDGE mode*/
const char hello_bridge_usb[] = "USB BRIDGE STARTED in debug mode!\r\n";

/** @brief VCP events callback to be registered in USB CDC stack through @ref usb_can_vcp_init() */
static const CdcCallbacks cdc_cb = {
    vcp_on_cdc_tx_complete,
    vcp_on_cdc_rx,
    vcp_state_callback,
    vcp_on_cdc_control_line,
    vcp_on_line_config,
};

/* ----------------------------------------------------------- GLOBAL VARIABLES--------------------------------------------------------------- */

/** @brief flag used to synchronize cdc (VCP) driver access. 
 * @details This flag is set when an USB CDC transaction is requested (via @ref WAIT_CDC)
 * and cleared when USB CDC transaction complete (on @ref vcp_on_cdc_tx_complete() call).
 * This is used instead of mutex because mutex usage during interruption provoke error.*/
static volatile bool cdc_busy;
/** @brief flag used to check CAN interface has been initialized before issuing a transaction. If it is not the case an error will be returned.*/
static bool can_if_initialized;
/** @brief flag set when 'd' custom command is sent in USB degub bridge mode used to add carriage return on CAN Rx frames */
static bool debug;

/* ----------------------------------------------------------- LOCAL FUNCTION IMPLEMENTATION--------------------------------------------------------------- */

/** @brief Function called on CAN RX IRQ. It will basically set a flag to signal to rx thread a packet has been received.
 * @details
*/
static void usb_can_on_irq_cb(void* context) {
    UsbCanBridge* app = (UsbCanBridge*)context;
    furi_thread_flags_set(furi_thread_get_id(app->thread), WorkerEvtRxDone);
    furi_hal_gpio_disable_int_callback(&gpio_ext_pc3);
}
/** 
 * @brief USB Virtual com port initialization.
**/
static void usb_can_vcp_init(UsbCanBridge* usb_can, uint8_t vcp_ch) {
    furi_hal_usb_unlock();
    furi_hal_cdc_set_callbacks(vcp_ch, (CdcCallbacks*)&cdc_cb, usb_can);
}

/** 
 * @brief USB Virtual com port close function called after a @ref usb_can_disable() or a @ref usb_can_set_config() call.
 * @details This function is called either :
 * - to suspend USB CDC driver usage during reconfiguration (VCP channel change)
 * - to free USB CDC driver ressources during an @ref usb_can_disable call.
**/
static void usb_can_vcp_deinit(UsbCanBridge* usb_can, uint8_t vcp_ch) {
    UNUSED(usb_can);
    furi_hal_cdc_set_callbacks(vcp_ch, NULL, NULL);
    if(vcp_ch != 0) {
        Cli* cli = (Cli*)furi_record_open(RECORD_CLI);
        cli_session_close(cli);
        furi_record_close(RECORD_CLI);
    }
}

/**
 * @brief function printing serma banner on VCP 
 * @details This function is called by @ref usb_can_worker() when user select USB-loopback or USB-CAN-bridge application mode in can-fd-hs menu (CAN-TEST does not use USB link). */
static void usb_can_print_banner(UsbCanBridge* usb_can) {
    uint32_t i;
    (void)usb_can;

    for(i = 0; i < sizeof(serma_banner) - CDC_DATA_SZ; i += CDC_DATA_SZ) {
        WAIT_CDC();
        furi_hal_cdc_send(usb_can->cfg.vcp_ch, (uint8_t*)&serma_banner[i], CDC_DATA_SZ);
    }
    WAIT_CDC();
    furi_hal_cdc_send(usb_can->cfg.vcp_ch, (uint8_t*)&serma_banner[i], sizeof(serma_banner) - i);
}

/**
 * @brief application main thread : receives CAN frames and forward it to host.
 * @details This thread is started upon application mode selection by user. It perform following actions:
 * -# if can is enabled (USB loopback test not selected)
 *     - initialize CAN @500KBaud through @ref mcp2518fd::begin()
 *     - create a timer through @ref furi_timer_alloc() to clear TX LED through @ref 
 *     - if CAN test is enabled , enter in an infinite transmit loop (exited only on @ref WorkerEvtStop) which will :
 *          - switch ON every 2 transmissions through @ref mcp2518::mcpDigitalWrite()
 *          - start timer @ 200ms through @ref furi_timer_start() to schedule LED clear and CAN transmissions through @ref usb_can_bridge_clear_led()
 *          - send test message (identifier 0x7E57CA, data="CANLIVE") through @ref mcp2518::sendMsgBuf()
 *          - wait either @ref usb_can_bridge_clear_led() is trigged ( @ref WorkerEvtTestTx) or app is leaved ( @ref WorkerEvtStop)
 * -# initialize USB Virtual Com port (USB CDC class) through @ref usb_can_vcp_init()
 * -# display Serma welcome banner on Virtual Com Port (VCP) through @ref usb_can_print_banner()
 * -# print application mode (USB loopback or USB-CAN-Bridge) 
 * -# allocate (through @ref furi_thread_alloc_ex()) and  start @ref usb_can_tx_thread() ( @ref UsbCanBridge::tx_thread) used for CAN transmission through @ref furi_thread_start().
 * -# start thread Main loop : This loop will block until either :
 *      - a packet is received on CAN Link : flag set via @ref usb_can_on_irq_cb(). in this case:
 *          -# CAN frame data field is read via @ref mcp2518fd::readMsgBuf()
 *          -# interrupt flags are cleared to deassert interrupt pin @ref mcp2518fd::mcp2518fd_readClearInterruptFlags(). This can not be done in interrupt mode because SPI communication is needed.
 *          -# GPIO used for interrupt is re-enabled @ref furi_hal_gpio_enable_int_callback()
 *          -# CAN ID is read through @ref mcp2518fd::getCanId() and interpreted through @ref mcp2518fd::isExtendedFrame()
 *          -# CAN frame is formated to SLCAN format (\<i\> identifier and \<d\> data):: 
 *              - for standard frames : tiildddd...=> @ref snprintf() format t%03X%1d%02X%02X... 
 *              - for extended frames : TIIIIIIIIlddd... @ref snprintf() format T%08X%1d%02X%02X ...
 *          -# formated SLCAN frame is sent on USB VCP through @ref furi_hal_cdc_send().
 *      - VCP is configured via flipper buttons via @ref usb_can_set_config()
 *  @warning 3 important points have to be considered:
 *  - ssnprintf() works bad with "%hhx" formatter (for 8 bytes types) if data to print is not 4 bytes aligned (ex: in an array), that's why an intermediary variable is used to print CAN Rx buffer.
 *  - @ref mcp2518fd::readMsgBuf() shall be called before clearing interrupt flags, otherwise flags will not be cleared.
 *  - as described in detailed description, clear of interrupt flag shall be done here. 
 * */
static int32_t usb_can_worker(void* context) {
    UsbCanBridge* usb_can = (UsbCanBridge*)context;
    uint32_t events = 0;
    uint32_t tmp_data;
    int printSize;
    byte len = 0;
    REG_CiINTENABLE flags;
    unsigned char buf[256];
    char tmp[32];
    const char msg_fmt[] = "t%03lX%1ld";
    const char ext_msg_fmt[] = "T%08lX%1ld";
    const char* fmt_pointer;

    if(usb_can->state != UsbCanLoopBackTestState) {
        furi_hal_gpio_init(&gpio_ext_pc3, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
        furi_hal_gpio_remove_int_callback(&gpio_ext_pc3);
        furi_hal_gpio_add_int_callback(&gpio_ext_pc3, usb_can_on_irq_cb, usb_can);
        furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_external);
        usb_can->can = new mcp2518fd(0);

        if(usb_can->state == UsbCanPingTestState) {
            const char test_can_msg[] = "CANLIVE";
            usb_can->can->begin(CAN20_500KBPS, CAN_SYSCLK_40M);
            usb_can->timer =
                furi_timer_alloc(usb_can_bridge_clear_led, FuriTimerTypeOnce, usb_can);
            while(!(events & WorkerEvtStop)) {
                if(usb_can->st.tx_cnt % (TEST_MESSAGE_LENGTH * 2)) {
                    usb_can->can->mcpDigitalWrite(GPIO_PIN_0, GPIO_LOW);
                }
                usb_can->st.tx_cnt += TEST_MESSAGE_LENGTH;
                furi_timer_start(usb_can->timer, furi_kernel_get_tick_frequency() / 5);
                usb_can->can->sendMsgBuf(
                    (uint32_t)0x7E57CA,
                    (byte)1,
                    (byte)TEST_MESSAGE_LENGTH,
                    (const byte*)test_can_msg);
                events = furi_thread_flags_wait(
                    WorkerEvtStop | WorkerEvtTestTx, FuriFlagWaitAny, FuriWaitForever);
            }
            return 0;
        }
    }
    memcpy(&usb_can->cfg, &usb_can->cfg_new, sizeof(UsbCanConfig));
    usb_can->rx_stream = furi_stream_buffer_alloc(USB_CAN_RX_BUF_SIZE, 1);

    usb_can->tx_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    usb_can->timer = furi_timer_alloc(usb_can_bridge_clear_led, FuriTimerTypeOnce, usb_can);

    usb_can->tx_thread = furi_thread_alloc_ex("UsbCanTxWorker", 2048, usb_can_tx_thread, usb_can);

    usb_can_vcp_init(usb_can, usb_can->cfg.vcp_ch);
    if(usb_can->state == UsbCanLoopBackTestState) {
        usb_can_print_banner(usb_can);
        WAIT_CDC();
        const char hello_test_usb[] = "USB LOOPBACK TEST START\r\n";
        furi_hal_cdc_send(usb_can->cfg.vcp_ch, (uint8_t*)hello_test_usb, sizeof(hello_test_usb));
    }

    furi_thread_start(usb_can->tx_thread);

    while(1) {
        events = furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check(!(events & FuriFlagError));
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            usb_can->can->readMsgBuf(&len, buf); // You should call readMsgBuff before getCanId
            usb_can->can->mcp2518fd_readClearInterruptFlags(&flags);

            unsigned long id = usb_can->can->getCanId();
            byte ext = usb_can->can->isExtendedFrame();
            tmp_data = len;

            if(ext) {
                fmt_pointer = ext_msg_fmt;
            } else {
                fmt_pointer = msg_fmt;
            }
            printSize = snprintf(tmp, sizeof(tmp), fmt_pointer, id, tmp_data);
            for(int i = 0; i < len; i++) {
                tmp_data = buf[i];
                printSize += snprintf(&tmp[printSize], 3, "%02lX", tmp_data);
            }
            printSize += snprintf(&tmp[printSize], 2, "\r");
            if(debug) {
                printSize += snprintf(&tmp[printSize], 2, "\n");
            }
            usb_can->st.rx_cnt += len;
            WAIT_CDC();
            furi_hal_cdc_send(usb_can->cfg.vcp_ch, (uint8_t*)tmp, printSize);
            furi_hal_gpio_enable_int_callback(&gpio_ext_pc3);
        }

        if(events & WorkerEvtCfgChange) {
            if(usb_can->cfg.vcp_ch != usb_can->cfg_new.vcp_ch) {
                furi_thread_flags_set(furi_thread_get_id(usb_can->tx_thread), WorkerEvtTxStop);
                furi_thread_join(usb_can->tx_thread);

                usb_can_vcp_deinit(usb_can, usb_can->cfg.vcp_ch);
                usb_can_vcp_init(usb_can, usb_can->cfg_new.vcp_ch);

                usb_can->cfg.vcp_ch = usb_can->cfg_new.vcp_ch;
                furi_thread_start(usb_can->tx_thread);
            }
            api_lock_unlock(usb_can->cfg_lock);
        }
    }
    usb_can_vcp_deinit(usb_can, usb_can->cfg.vcp_ch);

    furi_thread_flags_set(furi_thread_get_id(usb_can->tx_thread), WorkerEvtTxStop);
    furi_thread_join(usb_can->tx_thread);
    furi_thread_free(usb_can->tx_thread);

    furi_stream_buffer_free(usb_can->rx_stream);
    furi_mutex_free(usb_can->tx_mutex);

    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    Cli* cli = (Cli*)furi_record_open(RECORD_CLI);
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);

    return 0;
}

/**
 * @brief CAN TX thread : Process SLCAN commands send by the host.
 * @details This thread is started by @ref usb_can_worker(). It is composed by aloop which performs the following actions:
 * -# wait until a frame is received through USB VCP channel
 * -# If Application is in USB loopback mode it will just send back received frame and wait next USB frame.
 * -# Otherwise, if application is in USB-CAN-bridge mode it will process SLCAN commands in a loop as follows:     
 * <ol>
 *  <li>get next command in USB CDC reception buffer (delimited by next carriage return) by calling @ref _usb_uart_bridge_get_next_command()</li>
 *  <li>either call :</li>
 *      - @ref usb_can_bridge_configure() if it is a configuration Command (Sx)
 *      - @ref usb_can_bridge_send() if it is a transmit command (tiiildddd...., Tiiiiiiiilddddd..., riiil..., Riiiiiiiil.)
 *      - @ref usb_can_bridge_error() an send  " [err]: command <command>><x> not implemented yet!"" if command is not implemented.
 *      - @ref M_USB_CAN_ERROR_INVALID_COMMAND otherwise
 * </ol>*/
static int32_t usb_can_tx_thread(void* context) {
    UsbCanBridge* usb_can = (UsbCanBridge*)context;
    uint8_t data[USB_CDC_PKT_LEN];
    /** @brief constant used to signal user SLCAN command is invalid (the first character does not correspond to an SLCAN command) */
    const char invalid_command[] = "[err]: invalid command received '%c' in ascii (0x%02X)\r\n";
    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_TX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check(!(events & FuriFlagError));
        if(events & WorkerEvtTxStop) break;
        if(events & WorkerEvtCdcRx) {
            WAIT_CDC();
            size_t len = furi_hal_cdc_receive(usb_can->cfg.vcp_ch, data, USB_CDC_PKT_LEN);
            cdc_busy = false;
            if(usb_can->state == UsbCanLoopBackTestState) {
                usb_can->st.tx_cnt += len;
                WAIT_CDC();
                furi_hal_cdc_send(usb_can->cfg.vcp_ch, data, len);

            } else {
                char* next_command = (char*)NULL;
                uint8_t cmd_len = 0;
                while(
                    _usb_uart_bridge_get_next_command(&next_command, &cmd_len, (char*)data, len)) {
                    if(cmd_len > USB_CAN_BRIDGE_MAX_INPUT_LENGTH) {
                        M_USB_CAN_ERROR_INVALID_LENGTH();
                        //invalid input, stop processing
                        continue;
                    }

                    switch(next_command[0]) {
                    case '\n':
                    case '\r':
                        //ignore carriage return and line feed sent alone (ex. putty)=> this is not considered as an error
                        if(cmd_len != 1) {
                            M_USB_CAN_ERROR_INVALID_COMMAND();
                        }
                        break;
                    case 'S':
                        usb_can_bridge_configure(usb_can, (char*)next_command, cmd_len);
                        break;
                    case 'O':
                    case 'C':
                        usb_can_bridge_error(
                            usb_can, "[err]: just select app with the flipper buttons! \r\n");
                        break;
                    case 's':
                    case 'G':
                    case 'W':
                    case 'V':
                    case 'v':
                    case 'N':
                    case 'I':
                    case 'L':
                    case 'F':
                    case 'Z':
                        usb_can_bridge_error(
                            usb_can,
                            "[err]: command %c<x> not implemented yet!\r\n",
                            next_command[0]);
                        break;
                    case 'R':
                        usb_can_bridge_send(usb_can, next_command, cmd_len, true, true);
                        break;
                    case 'r':
                        usb_can_bridge_send(usb_can, next_command, cmd_len, false, true);
                        break;
                    case 'T':
                        usb_can_bridge_send(usb_can, next_command, cmd_len, true, false);
                        break;
                    case 't':
                        usb_can_bridge_send(usb_can, next_command, cmd_len, false, false);
                        break;
                    case 'd': {
                        usb_can_print_banner(usb_can);
                        WAIT_CDC();
                        furi_hal_cdc_send(
                            usb_can->cfg.vcp_ch,
                            (uint8_t*)hello_bridge_usb,
                            sizeof(hello_bridge_usb));
                        debug = true;
                    } break;
                    default:
                        M_USB_CAN_ERROR_INVALID_COMMAND();
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

/**
 * @brief Used to check if it remains bytes to read in USB CDC frame.
 * @details This Macro is called by @ref _usb_uart_bridge_get_next_command() function in
 * order to determine if it remains bytes to read in USB CDC frame. */
#define IS_LAST_CMD() (&((*next_command)[(*cmd_length)]) == &cdc_data[cdc_frame_len])
/**
 * @brief SLCAN command extraction.
 * @details This function updates @ref next_command pointer to point at the beggining of next SLCAN command.
 * This function performs the following actions:
 * -# if *next_command is null (new CDC frame)
 *      - set next_command to point the first byte of USB CDC (VCP) frame.
 * -# Otherwise:
 *      -  go to next command by updating @ref next_command pointer : it consists to move pointer at the end of previous command by adding @ref cmd_length value ( @ref cmd_length is used like an index).
 * -# reset cmd_length to 0
 * -# search next CR or LF USB character (relative to @ref next_command pointer) in CDC frame, incrementing each time @ref cmd_length which is used as an index.
 * -# if CDC frame last index is reached before finding a CR or an LF character, returns false
 * -# returns true otherwise after incrementing @ref command_length a last time to include termination character (CR or LF) .
 * @pre next_command shall be either Null (new CDC frame) or shall be called with: <ul><li>next_command pointer set to the last @ref _usb_uart_bridge_get_next_command() returned value.</li>
 * <li>cmd_length pointer set to the last @ref _usb_uart_bridge_get_next_command() returned value.</li></ul>
 * @param[inout] next_command When passed to the function, it correspond to a pointer to last processed USB CDC frame chunk (SLCAN command). At the end of the function it points to next SLCAN command.
 * @param[inout] cmd_length When passed to the function, it correspond to the length of last processed USB CDC frame chunk (SLCAN command) pointed by @ref next_command. At the end of the function it corresponds to the length of next SLCAN command.
 * @param[in] cdc_data Pointer to current USB CDC frame to be parsed. The first time this fonction is called (for a new cdc frame), @ref next_command is pointing to the same location than this parameter. Each time a valid command is parsed @ref next_command pointer move to next command location in this frame.
 * @param[in] cdc_frame_len Length of current CDC frame.
 * @return True if it remains commands to process in current USB CDC frame, False otherwise (next CDC frame is to be read).  
**/
static bool _usb_uart_bridge_get_next_command(
    char** next_command,
    uint8_t* cmd_length,
    char* cdc_data,
    uint8_t cdc_frame_len) {
    if(*next_command == NULL) {
        *next_command = cdc_data;
    } else {
        //This is not first command : update command pointer
        *next_command = &((*next_command)[*cmd_length]);
    }
    *cmd_length = 0;
    while(((*next_command)[(*cmd_length)] != '\r') && ((*next_command)[(*cmd_length)] != '\n') &&
          (!IS_LAST_CMD())) {
        (*cmd_length)++;
    }
    if(IS_LAST_CMD()) {
        return false;
    } else {
        (*cmd_length)++;
    }
    return true;
}

/**
 * @brief SLCAN S\<X\> command processing : CAN bus configuration
 * @details This function performs the following actions:
 * -# Check S command length is 3 (S\<X\>\\r). If it is not the case sends an error message on VCP (USB CDC channel) via @ref M_USB_CAN_ERROR_INVALID_LENGTH() before returning.
 * -# Check \<X\> is a digit. If it is not, sends an error message on VCP (USB CDC channel) by calling @ref usb_can_bridge_error() before returning.
 * -# configure CAN bus through @ref mcp2518fd::begin() function. @ref bitrate_table is used to map S<X> command parameter to @ref mcp2518fd::begin() @ref speedset parameter.
 * -# set @ref can_if_initialized to true, in order to be able to process Tx commands (through @ref usb_can_bridge_send()) normally.
 * @pre data parameter shall be an S command, or shall begin like an S command (with an 'S') : "Sxxxxxxxxx" 
 * @param[in] usb_can Application Control logic related data. It is used because it contains can driver instance (@ref mcp2518fd)
 * @param[in] data SLCAN S command.
 * @param[in] len command length (data string length).
**/
static void usb_can_bridge_configure(UsbCanBridge* usb_can, char* data, uint32_t len) {
    static const uint32_t bitrate_table[] = {
        CAN20_10KBPS,
        CAN20_20KBPS,
        CAN20_50KBPS,
        CAN20_100KBPS,
        CAN20_125KBPS,
        CAN20_250KBPS,
        CAN20_500KBPS,
        CAN20_800KBPS,
        CAN20_1000KBPS,
        CAN_500K_4M};

    uint32_t idx;

    if(len != 3) {
        M_USB_CAN_ERROR_INVALID_LENGTH();
    } else if(data[1] < '0' || data[1] > '9') {
        usb_can_bridge_error(
            usb_can, "invalid setting (command S): %c in ascii (character 0x%02X)\r\n", data[1]);
    } else {
        idx = atoi(&data[1]);
        usb_can->can->begin(bitrate_table[idx], CAN_SYSCLK_40M);
        can_if_initialized = true;
    }
}

/**
 * @brief SLCAN Transmit command processing (txxx,Txxxx,rxxxx,Rxxxx)
 * @details This function performs the following actions:
 * -# set header input format to be provided to @ref sscanf pasring function according to SLCAN command processed:
 *  - t%3x%1x for tiiil command
 *  - T%8x%1x for Tiiiiiiiil command
 *  - r%3x%1x for riiil command
 *  - R%8x%1x for Riiiiiiiil command
 * -# set max identifier valid value according to the SLCAN command : 1FFFFFFF for extended frames (command Tiiii... and Riii...), 0x7ff otherwise
 * -# set command length bounds according to the SLCAN command : 5 < length < 22 for standard frames (tiii..,riii...), 10 < length < 27 for extended frames.
 * -# check @ref can_if_initialized is true, in order to ensure CAN interface have been initialized (via @ref usb_can_bridge_configure). If it is not he case send an error message before returning via @ref usb_can_bridge_error.
 * -# check SLCAN command length is within the previously defined bounds. If it is not the case transmit error message before returning via @ref M_USB_CAN_ERROR_INVALID_LENGTH.
 * -# parse command header (from index 0 to min length) in order to get CAN ID and CAN data length code (DLC) through @ref sscanf. The format string computed in step1 is used.
 * -# determine length of data field : 0 if this is an RTR (riiil,Riiiiiiiil), =DLC for Tx (tiiildddd...,Tiiiiiiiilddddd...)
 * -# check DLC field value is consistent with SLCAN command length it shall be equal to header length (5 or 10 + data field length)
 *      - If it is not the case send error message "[err]: can payload length discrepency : expected <X> got <Y>" through @ref usb_can_bridge_error before retruning 
 * -# check if ID field value is valid (ie. inferior to value computed previously)
 * -# switch on Tx LED.
 * -# add previously computed data field length to Tx counter
 * -# parse and extract SLCAN command data field
 * -# transmit CAN frame through @ref mcp2518fd::sendMsgBuf() with previously extracted data, CAN ID and dlc fields
 * -# call timer to schedule Tx LED switch off.  
 * 
 * @pre data parameter shall be a Tx command, or shall begin like an Tx command (with a 'T', a 't', an 'R', or an 'r' ) 
 * @param[in] usb_can Application Control logic related data. It is used because it contains can driver instance.
 * @param[in] data SLCAN Tx command (riiil,Riiiiiiiil,tiiildddd...,Tiiiiiiiilddddd...)
 * @param[in] len command length (data string length)
 * @param[in] extended tell if CAN frame to be sent is an extended frame or not (SLCAN command begins with an uppercase letter)
 * @param[in] rtr tell if CAN frame to be sent is a standard TX or an RTR (Remote Transmission Request) frame (i.e SLCAN command begins with a 'T/t' or with an 'R/r') 
**/
static void
    usb_can_bridge_send(UsbCanBridge* usb_can, char* data, uint32_t len, bool extended, bool rtr) {
    uint8_t data_bin[8];
    uint32_t expected_len = 0;
    uint32_t id;
    uint32_t data_tmp;
    uint32_t max_identifier;
    uint8_t data_offset;
    uint32_t min_length;
    uint32_t max_length;
    char frame_format[] = "t%3x%1x";
    uint32_t dlc;

    if(extended) {
        max_identifier = 0x1FFFFFFF;
        data_offset = 10;
        if(rtr) {
            frame_format[0] = 'R';
        } else {
            frame_format[0] = 'T';
        }
        frame_format[2] = '8';
    } else {
        max_identifier = 0x7FF;
        data_offset = 5;
        if(rtr) {
            frame_format[0] = 'r';
        };
    }

    min_length = data_offset;
    max_length = min_length + 17;
    if(!can_if_initialized) {
        const char invalid_state[] = "[err]:  please initialize CAN using Sx command!\r\n";
        usb_can_bridge_error(usb_can, invalid_state, sizeof(invalid_state));
    } else if(len >= min_length && len <= max_length) {
        sscanf((const char*)&data[0], frame_format, (unsigned int*)&id, (unsigned int*)&dlc);
        if(!rtr) {
            expected_len = dlc;
        }
        if(id > max_identifier) {
            const char invalid_identifier[] = "[err]: invalid can identifier (>%X): %X \r\n";
            usb_can_bridge_error(usb_can, invalid_identifier, max_identifier, id);
        } else if((len - data_offset) / 2 == expected_len) {
            usb_can->can->mcpDigitalWrite(GPIO_PIN_0, GPIO_LOW);
            usb_can->st.tx_cnt += expected_len;
            for(uint32_t i = 0; i < expected_len; i++) {
                sscanf((const char*)&data[data_offset + 2 * i], "%02lx", &data_tmp);
                data_bin[i] = (uint8_t)(data_tmp & 0xFF);
            }
            usb_can->can->sendMsgBuf(id, extended, rtr, dlc, data_bin);

            furi_timer_start(usb_can->timer, furi_kernel_get_tick_frequency() / 100);
        } else {
            usb_can_bridge_error(
                usb_can,
                "[err]: can payload length discrepency : expected %d got %d  \r\n",
                expected_len,
                (len - data_offset) / 2);
        }
    } else {
        M_USB_CAN_ERROR_INVALID_LENGTH();
    }
}

/** @brief  clear CAN Tx LED 'set MCP2518 GPIO_0 high) through @ref mcp2518fd::mcpDigitalWrite() function.
 * @details This function recover allocated @ref UsbCanBridge::can (of type @ref mcp2518fd) instance created in @ref usb_can_enable() before calling  mcp2518fd::mcpDigitalWrite().
 * @param[in] context generic parameter to be passed in flipper callback registration. Here a pointer to application control flow ( @ref UsbCanBridge) logic data is passed, to be recovered in callbacks.*/
static void usb_can_bridge_clear_led(void* context) {
    (void)context;
    UsbCanBridge* usb_can = (UsbCanBridge*)context;
    usb_can->can->mcpDigitalWrite(GPIO_PIN_0, GPIO_HIGH);
    if(usb_can->state == UsbCanPingTestState) {
        furi_thread_flags_set(furi_thread_get_id(usb_can->thread), WorkerEvtTestTx);
    }
}

/** @brief function called on error : it is an implementation to printf which prints output on USB CDC channel.
 * @details This function :
 * -# parses variadic arguments through @ref va_start
 * -# create a string according to err_message format and parameters supplied (to print)
 * -# require USB CDC driver ressource (probably a buffer) through @ref WAIT_CDC()
 * -# prints the string obtained previously on USB CDC VCP via ( @ref furi_hal_cdc_send())
 * @param[in] usb_can structure holding VCP channel used by the application ( @ref UsbCanBridge::cfg -> UsbCanConfig::vcp_ch)
 * @param[in] err_msg_format string to create format : same format as printf. **Shall correspond to supplied variadic argument list in order to avoid unpredictable behavior.**
*/
static void usb_can_bridge_error(UsbCanBridge* usb_can, const char* err_msg_format, ...) {
    uint32_t printSize;
    char out_msg[64];
    va_list args;
    va_start(args, err_msg_format);
    printSize = vsnprintf(out_msg, 64, err_msg_format, args);
    WAIT_CDC();
    furi_hal_cdc_send(usb_can->cfg.vcp_ch, (uint8_t*)out_msg, printSize);
}

/* VCP callbacks */

/** @brief This function is a callback registered during USB CDC driver initialization. It permits to synchronize writes on VCP channel.
 * @details This function simply clear @ref cdc_busy flag to signal USB CDC driver is available for transmission. 
 * <br> This flag is used @ref CDC_WAIT to synchronize USB CDC transmission requests.
 * @param[in] context generic parameter to be passed in flipper callback registration. Here it is unused.
 */
static void vcp_on_cdc_tx_complete(void* context) {
    (void)context;
    cdc_busy = false;
}

/** @brief This function is a callback registered during USB CDC driver initialization. It permits to signal to @ref usb_can_tx_thread() data is available in USB CDC VCP.
 * @details This function simply clear @ref cdc_busy flag to signal USB CDC driver is available for transmission. 
 * <br> This flag is used @ref CDC_WAIT to synchronize USB CDC transmission requests.
 * @param[in] context generic parameter to be passed in flipper callback registration. Here it is unused.
 */
static void vcp_on_cdc_rx(void* context) {
    UsbCanBridge* usb_can = (UsbCanBridge*)context;
    furi_thread_flags_set(furi_thread_get_id(usb_can->tx_thread), WorkerEvtCdcRx);
}

/** @brief This function is a callback registered during USB CDC driver initialization. It is unused for the moment.
 * @details This function simply clear @ref cdc_busy flag to signal USB CDC driver is available for transmission. 
 */
static void vcp_state_callback(void* context, uint8_t state) {
    UNUSED(context);
    UNUSED(state);
}

/** @brief Callback registered during USB CDC driver initialization. It is not used here since we do not need virtual UART control signals (CTS, etc...).
 */
static void vcp_on_cdc_control_line(void* context, uint8_t state) {
    UNUSED(context);
    UNUSED(state);
}

/** @brief Callback registered during USB CDC driver initialization. It is not used here since we use dedicated SLCAN commands to configure CAN link.
 */
static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config) {
    UNUSED(context);
    UNUSED(config);
}
/* ----------------------------------------------------------- GLOBAL FUNCTIONS IMPLEMENTATION--------------------------------------------------------------- */
/** 
 * @brief Entry point of application's operating mode (cf. @ref usb_can_worker()) called when one of the can-fd-hs application menu item (USB-UART-Bridge, USB loopback, CAN test) is choosen.
 * @details This function is called via @ref usb_can_scene_usb_can_on_enter() when one of the flipper zero  can-fd-hs applications mode is chosen by the user.
 * It will:
 * -# allocate memory to store USB and CAN control logic internal data @ref UsbCanApp::usb_can_bridge ( @ref UsbCanBridge)
 * -# set application internal state ( @ref UsbCanBridge::state) according to application mode chosen by the user
 * -# start application main thread : @ref usb_can_worker().
 * @param[inout] app structure holding application related data (model, view, control logic)
 * @param[in] state application requested state. It correspond to application mode (menu item) chosen by the user.
**/
UsbCanBridge* usb_can_enable(UsbCanApp* app, usbCanState state) {
    app->usb_can_bridge = (UsbCanBridge*)malloc(sizeof(UsbCanBridge));
    app->usb_can_bridge->state = state;
    app->usb_can_bridge->thread =
        furi_thread_alloc_ex("UsbCanWorker", 2048, usb_can_worker, app->usb_can_bridge);

    furi_thread_start(app->usb_can_bridge->thread);
    return app->usb_can_bridge;
}

/** 
 * @brief function called to leave application operationnal mode (free memory).
 * @details this function is called when back button is pressed by the user through @ref usb_can_scene_usb_can_on_exit(). It performs the following actions:
 * -# sends killing signal to application main thread ( @ref usb_can_worker())
 * -# wait thread terminates
 * -# free memory (and ressources) related to application control logic allocated in @ref usb_can_disable()
 * @param[inout] usb_can memory structure holding application (USB and CAN) control logic to free.
**/
void usb_can_disable(UsbCanBridge* usb_can) {
    furi_check(usb_can);
    furi_thread_flags_set(furi_thread_get_id(usb_can->thread), WorkerEvtStop);
    furi_thread_join(usb_can->thread);
    furi_thread_free(usb_can->thread);
    free(usb_can);
}

/** 
 * @brief configure application used USB CDC VCP channel. It is called in @ref usb_can_scene_config_on_event() when user want to set VCP channel.
 * @details this sets a configuration request by setting @ref WorkerEvtCfgChange to signal application main thread (i.e @ref usb_can_worker()) a configuration change is requested.
 * @param[inout] usb_can memory structure holding application (USB and CAN) control logic to free.
 * @param[in] cfg configuration to set.
**/
void usb_can_set_config(UsbCanBridge* usb_can, UsbCanConfig* cfg) {
    furi_check(usb_can);
    furi_check(cfg);
    usb_can->cfg_lock = api_lock_alloc_locked();
    memcpy(&(usb_can->cfg_new), cfg, sizeof(UsbCanConfig));
    furi_thread_flags_set(furi_thread_get_id(usb_can->thread), WorkerEvtCfgChange);
    api_lock_wait_unlock_and_free(usb_can->cfg_lock);
}

/** 
 * @brief USB VCP channel config (channel number used) getter (cf. @ref UsbCanBridge::cfg_new field) used for display purpose.
**/
void usb_can_get_config(UsbCanBridge* usb_can, UsbCanConfig* cfg) {
    furi_check(usb_can);
    furi_check(cfg);
    memcpy(cfg, &(usb_can->cfg_new), sizeof(UsbCanConfig));
}

/** 
 * @brief Application mode getter (cf. @ref UsbCanBridge::st field) used for display purpose.
**/
void usb_can_get_state(UsbCanBridge* usb_can, UsbCanStatus* st) {
    furi_check(usb_can);
    furi_check(st);
    memcpy(st, &(usb_can->st), sizeof(UsbCanStatus));
}
