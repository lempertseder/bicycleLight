/*
 * debugLog.h
 *
 *  Created on: Dec 13, 2025
 *      Author: m.lempertseder
 * 
 * Licensed under the MIT License.
 * See LICENSE and file headers for details.
 */

#ifndef SRC_DEBUGLOG_H_
#define SRC_DEBUGLOG_H_

#include "stm32c0xx_hal.h"
#include <stdint.h>

#define debugLog_Pin        GPIO_PIN_14
#define debugLog_GPIO_Port  GPIOC

void sendDebugByte( uint8_t sendByte );

#endif /* SRC_DEBUGLOG_H_ */
