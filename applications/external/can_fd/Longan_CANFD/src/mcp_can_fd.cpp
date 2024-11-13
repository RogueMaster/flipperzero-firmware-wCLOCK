#include "mcp_can_fd.h"

/*********************************************************************************************************
** Function name:           MCP_CAN_1
** Descriptions:            Constructor
*********************************************************************************************************/
MCP_CAN_1::MCP_CAN_1(byte _CS) {
    pSPI = &SPI;
    init_CS(_CS);
}

/*********************************************************************************************************
** Function name:           init_CS
** Descriptions:            init CS pin and set UNSELECTED
*********************************************************************************************************/
void MCP_CAN_1::init_CS(byte _CS) {
    if(_CS == 0) {
        return;
    }
    SPICS = _CS;
    pinMode(SPICS, OUTPUT);
    digitalWrite(SPICS, HIGH);
}

void MCP_CAN_1::setSPI(SPIClass* _pSPI) {
    pSPI = _pSPI; // define SPI port to use before begin()
}
