/** 
 * @ingroup CAN-DRIVER-ADAPTATION
 * @file Arduino.h
 * @brief used to not provoke inclusion error in Longan labs library. delay() macro is redefined to call furi delay function.
 * 
 **/
#include "../../../furi/core/kernel.h"
/** Arduino delay() macro porting */
#define delay(X) furi_delay_ms(X)
