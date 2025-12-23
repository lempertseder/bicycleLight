/*
 * debugLog.c
 *
 *  Created on: Dec 13, 2025
 *      Author: m.lempertseder
 * 
 * Licensed under the MIT License.
 * See LICENSE and file headers for details.
 */

#include "debugLog.h"

void sendDebugByte( uint8_t sendByte ){
	int i = 8;
	HAL_Delay(15);
	while (i>0){
		if (sendByte & 0x80){
			HAL_GPIO_WritePin(debugLog_GPIO_Port, debugLog_Pin, GPIO_PIN_SET);
			HAL_Delay(7);
			HAL_GPIO_WritePin(debugLog_GPIO_Port, debugLog_Pin, GPIO_PIN_RESET);
			HAL_Delay(3);
		}
		else {
			HAL_GPIO_WritePin(debugLog_GPIO_Port, debugLog_Pin, GPIO_PIN_SET);
			HAL_Delay(3);
			HAL_GPIO_WritePin(debugLog_GPIO_Port, debugLog_Pin, GPIO_PIN_RESET);
			HAL_Delay(7);
		}
		sendByte = sendByte << 1;
		i--;
	}
}
