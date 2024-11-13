/*  MCP2517/8 receive a CAN-FD frame with FILTER AND MASK SETTING

    CAN FD Shield - https://www.longan-labs.cc/1030012.html
    CANBed FD - https://www.longan-labs.cc/1030009.html
  
    can-fd baud rate:

    CAN_125K_500K
    CAN_250K_500K
    CAN_250K_750K
    CAN_250K_1M
    CAN_250K_1M5
    CAN_250K_2M
    CAN_250K_3M
    CAN_250K_4M
    CAN_500K_1M
    CAN_500K_2M
    CAN_500K_3M
    CAN_500K_4M
    CAN_1000K_4M

*/

#include "SPI.h"
#include "mcp2518fd_can.h"

#define MAX_DATA_SIZE 64

// pin for CAN-FD Shield
//const int SPI_CS_PIN = 9;
//const int CAN_INT_PIN = 2;

// pin for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[MAX_DATA_SIZE];

void setup() {
    Serial.begin(115200);
    while(!Serial);

    CAN.setMode(CAN_NORMAL_MODE);

    // init can bus : arbitration bitrate = 500k, data bitrate = 1M
    while(0 != CAN.begin(CAN_500K_4M)) {
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");

    // init_Filt_Mask(filter number, ext, filter, mask)
    // There're 32 set of filter/mask for MCP2517FD, filter number can be set to 0~31
    CAN.init_Filt_Mask(0, 0, 0, 0); // get all standard frame
    CAN.init_Filt_Mask(1, 1, 0, 0); // get all extended frame
}

void loop() {
    if(CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(&len, buf); // You should call readMsgBuff before getCanId
        unsigned long id = CAN.getCanId();

        Serial.print("Get Data From id: ");
        Serial.println(id);
        Serial.print("Len = ");
        Serial.println(len);
        // print the data
        for(int i = 0; i < len; i++) {
            Serial.print(buf[i]);
            Serial.print("\t");
        }
        Serial.println();
    }
}

// END FILE
