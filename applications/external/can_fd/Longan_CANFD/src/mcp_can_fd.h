#ifndef _MCP_CAN_H_
#define _MCP_CAN_H_

#include "Arduino.h"
#include "SPI.h"
#include <inttypes.h>
#include <stddef.h>

#define CAN_OK             (0)
#define CAN_FAILINIT       (1)
#define CAN_FAILTX         (2)
#define CAN_MSGAVAIL       (3)
#define CAN_NOMSG          (4)
#define CAN_CTRLERROR      (5)
#define CAN_GETTXBFTIMEOUT (6)
#define CAN_SENDMSGTIMEOUT (7)
#define CAN_FAIL           (0xff)

// clock
typedef enum {
    MCP_NO_MHz,
    /* apply to MCP2515 */
    MCP_16MHz,
    MCP_8MHz,
    /* apply to MCP2518FD */
    MCP2518FD_40MHz = MCP_16MHz /* To compatible MCP2515 shield */,
    MCP2518FD_20MHz,
    MCP2518FD_10MHz,
} MCP_CLOCK_T;

typedef enum {
    CAN_NOBPS,
    CAN20_5KBPS,
    CAN20_10KBPS,
    CAN20_20KBPS,
    CAN20_25KBPS,
    CAN20_31K25BPS,
    CAN20_33KBPS,
    CAN20_40KBPS,
    CAN20_50KBPS,
    CAN20_80KBPS,
    CAN20_83K3BPS,
    CAN20_95KBPS,
    CAN20_100KBPS,
    CAN20_125KBPS,
    CAN20_200KBPS,
    CAN20_250KBPS,
    CAN20_500KBPS,
    CAN20_666KBPS,
    CAN20_800KBPS,
    CAN20_1000KBPS
} MCP_BITTIME_SETUP;

class MCP_CAN_1 {
public:
    virtual void enableTxInterrupt(bool enable = true) = 0; // enable transmit interrupt
    virtual void reserveTxBuffers(byte nTxBuf = 0) = 0;
    virtual byte getLastTxBuffer() = 0;
    /*
     * speedset be in MCP_BITTIME_SETUP
     * clockset be in MCP_CLOCK_T
     */
    virtual byte begin(uint32_t speedset, const byte clockset) = 0; // init can
    virtual void setSleepWakeup(byte enable) = 0; // Enable or disable the wake up interrupt
        // (If disabled the MCP2515 will not be woken up by CAN bus activity, making it send only)
    virtual byte sleep() = 0; // Put the MCP2515 in sleep mode
    virtual byte wake() = 0; // Wake MCP2515 manually from sleep
    virtual byte __setMode(byte opMode) = 0; // Set operational mode
    virtual byte getMode() = 0; // Get operational mode
    virtual byte checkError(uint8_t* err_ptr = NULL) = 0; // if something error

    /* ---- receiving ---- */
    virtual byte checkReceive(void) = 0; // if something received
    virtual byte readMsgBufID(
        byte status,
        volatile unsigned long* id,
        volatile byte* ext,
        volatile byte* rtr,
        volatile byte* len,
        volatile byte* buf) = 0; // read buf with object ID
    /* wrapper */
    virtual byte readMsgBufID(unsigned long* ID, byte* len, byte* buf) = 0;
    virtual byte readMsgBuf(byte* len, byte* buf) = 0;

    /* could be called after a successful readMsgBufID() */
    unsigned long getCanId(void) {
        return can_id;
    }
    byte isRemoteRequest(void) {
        return rtr;
    }
    byte isExtendedFrame(void) {
        return ext_flg;
    }

    /* ---- sending ---- */
    virtual byte trySendMsgBuf(
        unsigned long id,
        byte ext,
        byte rtr,
        byte len,
        const byte* buf,
        byte iTxBuf = 0xff) = 0; // as sendMsgBuf, but does not have any wait for free buffer
    virtual byte sendMsgBuf(
        byte status,
        unsigned long id,
        byte ext,
        byte rtr,
        byte len,
        volatile const byte* buf) = 0; // send message buf by using parsed buffer status
    virtual byte sendMsgBuf(
        unsigned long id,
        byte ext,
        byte rtrBit,
        byte len,
        const byte* buf,
        bool wait_sent = true) = 0; // send message with wait

    virtual void
        clearBufferTransmitIfFlags(byte flags = 0) = 0; // Clear transmit flags according to status
    virtual byte readRxTxStatus(void) = 0; // read has something send or received
    virtual byte checkClearRxStatus(
        byte* status) = 0; // read and clear and return first found rx status bit
    virtual byte checkClearTxStatus(
        byte* status,
        byte iTxBuf =
            0xff) = 0; // read and clear and return first found or buffer specified tx status bit
    virtual bool mcpPinMode(
        const byte pin,
        const byte mode) = 0; // switch supported pins between HiZ, interrupt, output or input
    virtual bool
        mcpDigitalWrite(const byte pin, const byte mode) = 0; // write HIGH or LOW to RX0BF/RX1BF
    virtual byte mcpDigitalRead(const byte pin) = 0; // read HIGH or LOW from supported pins

public:
    MCP_CAN_1(byte _CS);
    void init_CS(byte _CS); // define CS after construction before begin()
    void setSPI(SPIClass* _pSPI);

protected:
    byte ext_flg; // identifier xxxID
    // either extended (the 29 LSB) or standard (the 11 LSB)
    unsigned long can_id; // can id
    byte rtr; // is remote frame
    byte SPICS;
    SPIClass* pSPI;
    byte mcpMode; // Current controller mode
};

#endif
