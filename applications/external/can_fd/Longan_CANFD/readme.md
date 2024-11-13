# Longan Labs CAN FD Library 

[![Actions Status](https://github.com/arduino/arduino-cli-example/workflows/test/badge.svg)](https://github.com/arduino/arduino-cli-example/actions)
[![Spell Check](https://github.com/arduino/compile-sketches/workflows/Spell%20Check/badge.svg)](https://github.com/arduino/compile-sketches/actions?workflow=Spell+Check)
[![codecov](https://codecov.io/gh/arduino/compile-sketches/branch/main/graph/badge.svg?token=Uv6f1ebMZ4)](https://codecov.io/gh/arduino/compile-sketches)

Arduino library for MCP2517/MCP2518, it's available for most of theArduino boards, we test it with Arduino UNO, Leonardo, Mega as well as Zero.

With this library, you can,

1. Send a CAN2.0 frame
2. Receive a CAN2.0 frame
3. Send a CAN FD frame
4. Send a CAN FD frame
5. Set the masks and filters, there're 32 masks and filters. 

## Installation

1. [Download the library](https://github.com/Longan-Labs/Longan_CANFD/archive/refs/heads/master.zip)
2. Extract the zip file
3. In the Arduino IDe, navigate to Sketch > Include Library > Add .ZIP Library

## Respository Contents

* [**/examples**](./examples) - Example sketches for the library (.ino). Run these from the Arduino IDE.
* [**/src**](./src) - Source files for the library (.cpp, .h).
* [**keywords.txt**](./keywords.txt) - Keywords from this library that will be highlighted in the Arduino IDE.
* [**library.properties**](./library.properties) - General library properties for the Arduino package manager.

## Functions

- begin()
- init_Filt_Mask()
- checkReceive()
- readMsgBufID()
- readMsgBuf()
- getCanId()
- sendMsgBuf()
- isRemoteRequest()
- isExtendedFrame()

## Examples

here are many examples implemented in this library. One of the examples is below. You can find other examples [here](./examples)

```Cpp
/*  MCP2517/8 send a can fd frame
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
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;

// pin for CANBed FD
//const int SPI_CS_PIN = 17;
//const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

unsigned char stmp[MAX_DATA_SIZE] = {0};


void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    CAN.setMode(CAN_NORMAL_MODE);

    // init can bus : arbitration bitrate = 500k, data bitrate = 1M
    while (0 != CAN.begin(CAN_500K_4M)) {
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");

    byte mode = CAN.getMode();
    Serial.print("CAN mode = ");
    Serial.println(mode);
    
    for(int i=0; i<MAX_DATA_SIZE; i++)
    {
        stmp[i] = i;
    }
}


void loop() 
{
    // send data:  id = 0x00, standrad frame, data len = 64, stmp: data buf
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    delay(10);
    CAN.sendMsgBuf(0x04, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    delay(500);                       // send data per 100ms
    Serial.println("CAN BUS sendMsgBuf ok!");
}

// END FILE
```

## Get a Dev Board

If you need a Dev board, plese try,

- [CAN FD Shield for Arduino](https://www.longan-labs.cc/1030012.html)
- [CANBed FD](https://www.longan-labs.cc/1030009.html)

## License

```
MIT License

Copyright (c) 2018 @ Longan Labs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Contact us

If you have any question, please feel free to contact [support@longan-labs.cc](support@longan-labs.cc)
