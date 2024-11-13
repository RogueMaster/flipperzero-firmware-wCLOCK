/*************************************************************************************************
    OBD-II_PIDs TEST CODE
    Loovee, Longan Labs 2022

    Query
    send id: 0x7df
      dta: 0x02, 0x01, PID_CODE, 0, 0, 0, 0, 0

    Response
    From id: 0x7E9 or 0x7EA or 0x7EB
      dta: len, 0x41, PID_CODE, byte0, byte1(option), byte2(option), byte3(option), byte4(option)

    https://en.wikipedia.org/wiki/OBD-II_PIDs
***************************************************************************************************/

#include "SPI.h"
#include "mcp2518fd_can.h"

// pin for CAN-FD Shield
//const int SPI_CS_PIN = 9;
//const int CAN_INT_PIN = 2;

// pin for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

#define PID_ENGIN_PRM 0x0C
#define PID_VEHICLE_SPEED 0x0D
#define PID_COOLANT_TEMP 0x05

#define CAN_ID_PID 0x7DF

unsigned char PID_INPUT;
unsigned char getPid = 0;

void set_mask_filt() {
    CAN.init_Filt_Mask(0, 0, 0x7E8, 0x7FC);
}

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}

bool getRPM(int* r) {
    sendPid(PID_ENGIN_PRM);
    unsigned long __timeout = millis();

    while(millis() - __timeout < 1000) // 1s time out
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if(CAN_MSGAVAIL == CAN.checkReceive()) { // check if get data
            CAN.readMsgBuf(&len, buf); // read data,  len: data length, buf: data buf

            if(buf[1] == 0x41) {
                *r = (256 * buf[3] + buf[4]) / 4;
                return 1;
            }
        }
    }

    return 0;
}

void setup() {
    Serial.begin(115200);
    while(!Serial) {
    };

    while(CAN_OK != CAN.begin(CAN20_500KBPS)) { // init can bus : baudrate = 500k
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
    set_mask_filt();
}

void loop() {
    int __rpm = 0;

    int ret = getRPM(&__rpm);

    if(ret) {
        Serial.print("Engin Speed: ");
        Serial.print(__rpm);
        Serial.println(" rpm");
    } else
        Serial.println("get Engin Speed Fail...");

    delay(500);
}

// END FILE