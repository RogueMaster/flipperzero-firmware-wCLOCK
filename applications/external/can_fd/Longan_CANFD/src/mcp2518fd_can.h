

#ifndef _MCP2518FD_H_
#define _MCP2518FD_H_
#include "mcp2518fd_can_dfs.h"
#include "Arduino.h"
#include "SPI.h"

// *****************************************************************************
// *****************************************************************************
//! Reset DUT

// Code anchor for break points
#define Nop() asm("nop")

// Index to SPI channel
// Used when multiple MCP25xxFD are connected to the same SPI interface, but
// with different CS
#define SPI_DEFAULT_BUFFER_LENGTH 96

//extern SPIClass* pSPI;
#define spi_readwrite(X)   pSPI->transfer(X)
#define spi_read()         spi_readwrite(0x00)
#define spi_write(spi_val) spi_readwrite(spi_val)
#define SPI_BEGIN()        pSPI->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0))
#define SPI_END()          pSPI->endTransaction();

// *****************************************************************************
// *****************************************************************************
// Section: Defines

#define CRCBASE  0xFFFF
#define CRCUPPER 1

//! Reverse order of bits in byte
const uint8_t BitReverseTable256[256] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70,
    0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8,
    0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34,
    0xB4, 0x74, 0xF4, 0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52,
    0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A,
    0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA, 0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16,
    0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61,
    0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1, 0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9,
    0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25,
    0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
    0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD, 0x03, 0x83, 0x43,
    0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B,
    0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07,
    0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F,
    0xFF};

//! Look-up table for CRC calculation
const uint16_t crc16_table[256] = {
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 0x8033, 0x0036, 0x003C, 0x8039,
    0x0028, 0x802D, 0x8027, 0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
    0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099,
    0x0088, 0x808D, 0x8087, 0x0082, 0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA,
    0x81FB, 0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C, 0x8179,
    0x0168, 0x816D, 0x8167, 0x0162, 0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101, 0x8303, 0x0306, 0x030C, 0x8309,
    0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C, 0x8359,
    0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2, 0x83A3, 0x03A6, 0x03AC, 0x83A9,
    0x03B8, 0x83BD, 0x83B7, 0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
    0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243, 0x0246, 0x024C, 0x8249,
    0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219,
    0x0208, 0x820D, 0x8207, 0x0202};

class mcp2518fd : public MCP_CAN_1 {
public:
    mcp2518fd(byte _CS)
        : MCP_CAN_1(_CS)
        , nReservedTx(0) {};

public:
    int8_t mcp2518fd_readClearInterruptFlags(REG_CiINTENABLE* flags);
    virtual void enableTxInterrupt(bool enable = true); // enable transmit interrupt
    virtual void reserveTxBuffers(byte nTxBuf = 0) {
        nReservedTx = (nTxBuf < 3 ? nTxBuf : 3 - 1);
    }
    virtual byte getLastTxBuffer() {
        return 3 - 1; // read index of last tx buffer
    }
    /*
   * speedset could be in MCP_BITTIME_SETUP,
   *          or fill by CANFD::BITRATE()
   */
    virtual byte begin(uint32_t speedset,
                       const byte clockset = MCP2518FD_20MHz); // init can

    bool __flgFDF = 0; // 1: FD, 0: 2.0

    virtual byte CANFDSPI_FilterDisable(CAN_FILTER filter);

    virtual byte init_Filt_Mask(byte num, byte ext, unsigned long f, unsigned long m);

    virtual void setSleepWakeup(const byte enable);
    virtual byte sleep();
    virtual byte wake();
    virtual byte __setMode(const byte opMode);

    virtual byte setMode(const byte opMode) {
        if((CAN_OPERATION_MODE)opMode !=
           CAN_SLEEP_MODE) { // if going to sleep, the value stored in opMode is not
            // changed so that we can return to it later
            mcpMode = (CAN_OPERATION_MODE)opMode;
        }

        return (byte)mcpMode;
    }
    virtual byte getMode();
    virtual byte checkError(uint8_t* err_ptr = NULL);

    /* ---- receiving ---- */
    virtual byte checkReceive(void);
    virtual byte readMsgBufID(
        byte status,
        volatile unsigned long* id,
        volatile byte* ext,
        volatile byte* rtr,
        volatile byte* len,
        volatile byte* buf); // read buf with object ID
    /* wrapper */
    byte readMsgBufID(unsigned long* ID, byte* len, byte* buf) {
        return readMsgBufID(readRxTxStatus(), ID, &ext_flg, &rtr, len, buf);
    }
    byte readMsgBuf(byte* len, byte* buf) {
        return readMsgBufID(readRxTxStatus(), &can_id, &ext_flg, &rtr, len, buf);
    }

    /* ---- sending ---- */
    /* dlc = CAN_DLC_0..CAN_DLC_64(0..15)
   *       CAN_DLC_0..CAN_DLC_8(0..8) = data bytes count, compatible to MCP2515 APIs
   *       CAN_DLC_12(9)              = 12 bytes
   *       CAN_DLC_16(10)             = 16 bytes
   *       CAN_DLC_20(11)             = 20 bytes
   *       CAN_DLC_24(12)             = 24 bytes
   *       CAN_DLC_32(13)             = 32 bytes
   *       CAN_DLC_48(14)             = 48 bytes
   *       CAN_DLC_64(15)             = 64 bytes
   */
    virtual byte trySendMsgBuf(
        unsigned long id,
        byte ext,
        byte rtr,
        byte dlc,
        const byte* buf,
        byte iTxBuf = 0xff);
    virtual byte sendMsgBuf(
        byte status,
        unsigned long id,
        byte ext,
        byte rtr,
        byte dlc,
        volatile const byte* buf);
    virtual byte sendMsgBuf(
        unsigned long id,
        byte ext,
        byte rtr,
        byte dlc,
        const byte* buf,
        bool wait_sent = true);
    /* wrapper */
    inline byte sendMsgBuf(unsigned long id, byte ext, byte len, const byte* buf) {
        return sendMsgBuf(id, ext, 0, len, buf, true);
    }

    virtual void clearBufferTransmitIfFlags(byte flags = 0);
    virtual byte readRxTxStatus(void);
    virtual byte checkClearRxStatus(byte* status);
    virtual byte checkClearTxStatus(byte* status, byte iTxBuf = 0xff);
    virtual bool mcpPinMode(const byte pin, const byte mode);
    virtual bool mcpDigitalWrite(const byte pin, const byte mode);
    virtual byte mcpDigitalRead(const byte pin);

private:
    byte mcp2518fd_readMsgBufID(volatile byte* len, volatile byte* buf);
    byte mcp2518fd_sendMsg(
        const byte* buf,
        byte len,
        unsigned long id,
        byte ext,
        byte rtr,
        bool wait_sent);
    int8_t mcp2518fd_receiveMsg();

private:
    uint8_t mcp2518fd_init(uint32_t speedset, const byte clock); // mcp2518fdinit
    int8_t mcp2518fd_reset(void); // reset mcp2518fd
    int8_t mcp2518fd_EccEnable(void); // Section: ECC
    int8_t mcp2518fd_RamInit(uint8_t d);
    int8_t mcp2518fd_ConfigureObjectReset(CAN_CONFIG* config);
    int8_t mcp2518fd_Configure(CAN_CONFIG* config);
    int8_t mcp2518fd_TransmitChannelConfigureObjectReset(CAN_TX_FIFO_CONFIG* config);
    int8_t
        mcp2518fd_TransmitChannelConfigure(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_CONFIG* config);
    int8_t mcp2518fd_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config);
    int8_t mcp2518fd_ReceiveChannelConfigure(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_CONFIG* config);
    int8_t mcp2518fd_FilterObjectConfigure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id);
    int8_t mcp2518fd_FilterMaskConfigure(CAN_FILTER filter, CAN_MASKOBJ_ID* mask);
    int8_t mcp2518fd_FilterToFifoLink(CAN_FILTER filter, CAN_FIFO_CHANNEL channel, bool enable);
    int8_t
        mcp2518fd_BitTimeConfigure(uint32_t speedset, CAN_SSP_MODE sspMode, CAN_SYSCLK_SPEED clk);
    int8_t mcp2518fd_GpioModeConfigure(GPIO_PIN_MODE gpio0, GPIO_PIN_MODE gpio1);
    int8_t mcp2518fd_TransmitChannelEventEnable(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT flags);
    int8_t mcp2518fd_ReceiveChannelEventEnable(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_EVENT flags);
    int8_t mcp2518fd_ModuleEventEnable(CAN_MODULE_EVENT flags);
    int8_t mcp2518fd_OperationModeSelect(CAN_OPERATION_MODE opMode);
    CAN_OPERATION_MODE mcp2518fd_OperationModeGet();
    void mcp2518fd_TransmitMessageQueue();
    int8_t mcp2518fd_TransmitChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT* flags);
    int8_t mcp2518fd_ErrorCountStateGet(uint8_t* tec, uint8_t* rec, CAN_ERROR_STATE* flags);
    int8_t mcp2518fd_TransmitChannelLoad(
        CAN_FIFO_CHANNEL channel,
        CAN_TX_MSGOBJ* txObj,
        uint8_t* txd,
        uint32_t txdNumBytes,
        bool flush);
    int8_t mcp2518fd_ReceiveChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_EVENT* flags);
    int8_t mcp2518fd_ReceiveMessageGet(
        CAN_FIFO_CHANNEL channel,
        CAN_RX_MSGOBJ* rxObj,
        uint8_t* rxd,
        uint8_t nBytes);
    int8_t mcp2518fd_ReceiveChannelUpdate(CAN_FIFO_CHANNEL channel);
    int8_t mcp2518fd_TransmitChannelUpdate(CAN_FIFO_CHANNEL channel, bool flush);
    int8_t mcp2518fd_ReceiveChannelStatusGet(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_STATUS* status);
    int8_t mcp2518fd_ErrorStateGet(CAN_ERROR_STATE* flags);
    int8_t mcp2518fd_ModuleEventRxCodeGet(CAN_RXCODE* rxCode);
    int8_t mcp2518fd_ModuleEventTxCodeGet(CAN_TXCODE* txCode);
    int8_t mcp2518fd_TransmitChannelEventAttemptClear(CAN_FIFO_CHANNEL channel);

    int8_t mcp2518fd_LowPowerModeEnable();
    int8_t mcp2518fd_LowPowerModeDisable();

    int8_t mcp2518fd_ReadByte(uint16_t address, uint8_t* rxd);
    int8_t mcp2518fd_WriteByte(uint16_t address, uint8_t txd);
    int8_t mcp2518fd_ReadWord(uint16_t address, uint32_t* rxd);
    int8_t mcp2518fd_WriteWord(uint16_t address, uint32_t txd);
    int8_t mcp2518fd_ReadHalfWord(uint16_t address, uint16_t* rxd);
    int8_t mcp2518fd_WriteHalfWord(uint16_t address, uint16_t txd);
    int8_t mcp2518fd_ReadByteArray(uint16_t address, uint8_t* rxd, uint16_t nBytes);
    int8_t mcp2518fd_WriteByteArray(uint16_t address, uint8_t* txd, uint16_t nBytes);
    int8_t mcp2518fd_WriteByteSafe(uint16_t address, uint8_t txd);
    int8_t mcp2518fd_WriteWordSafe(uint16_t address, uint32_t txd);
    int8_t mcp2518fd_ReadByteArrayWithCRC(
        uint16_t address,
        uint8_t* rxd,
        uint16_t nBytes,
        bool fromRam,
        bool* crcIsCorrect);
    int8_t mcp2518fd_WriteByteArrayWithCRC(
        uint16_t address,
        uint8_t* txd,
        uint16_t nBytes,
        bool fromRam);
    int8_t mcp2518fd_ReadWordArray(uint16_t address, uint32_t* rxd, uint16_t nWords);
    int8_t mcp2518fd_WriteWordArray(uint16_t address, uint32_t* txd, uint16_t nWords);

private:
    int8_t mcp2518fd_BitTimeConfigureNominal(void);
    int8_t mcp2518fd_BitTimeConfigureData(CAN_SSP_MODE sspMode);

    byte nReservedTx; // Count of tx buffers for reserved send
    CAN_OPERATION_MODE mcpMode = CAN_CLASSIC_MODE; // Current controller mode

    uint32_t bittime_compat_to_mcp2518fd(uint32_t speedset);
    int calcBittime(
        const uint32_t inDesiredArbitrationBitRate,
        const uint32_t inTolerancePPM = 10000 /* 1% */);

    uint32_t mSysClock; // PLL disabled, mSysClock = Oscillator Frequency
    uint32_t mDesiredArbitrationBitRate; // desired ArbitrationBitRate
    uint8_t mDataBitRateFactor; // multiplier between ArbitrationBitRate and DataBitrate
    //--- Data bit rate; if mDataBitRateFactor==1, theses properties are not used for configuring the MCP2517FD.
    uint8_t mDataPhaseSegment1 =
        0; // if mDataBitRateFactor > 1: 2...32, else equal to mArbitrationPhaseSegment1
    uint8_t mDataPhaseSegment2 =
        0; // if mDataBitRateFactor > 1: 1...16, else equal to mArbitrationPhaseSegment2
    uint8_t mDataSJW = 0; // if mDataBitRateFactor > 1: 1...16, else equal to mArbitrationSJW
    //--- Bit rate prescaler is common to arbitration and data bit rates
    uint16_t mBitRatePrescaler = 0; // 1...256
    //--- Arbitration bit rate
    uint16_t mArbitrationPhaseSegment1 = 0; // 2...256
    uint8_t mArbitrationPhaseSegment2 = 0; // 1...128
    uint8_t mArbitrationSJW = 0; // 1...128
    bool mArbitrationBitRateClosedToDesiredRate = false; // The above configuration is not correct
    //--- Transmitter Delay Compensation Offset
    int8_t mTDCO = 0; // -64 ... +63
};

/* CANFD Auxiliary helper */
class CANFD {
public:
    static byte dlc2len(byte dlc);
    static byte len2dlc(byte len);
    static uint32_t BITRATE(uint32_t arbitration, uint8_t factor) {
        return ((uint32_t)factor << 24) | (arbitration & 0xFFFFFUL);
    }
};

#endif
