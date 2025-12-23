/*
 * i2c_bitbang.h
 *
 *  Created on: Dec 13, 2025
 *      Author: m.lempertseder
 * 
 * Licensed under the MIT License.
 * See LICENSE and file headers for details.
 */

#ifndef SRC_I2C_BITBANG_H_
#define SRC_I2C_BITBANG_H_

#include "stm32c0xx_hal.h"
#include <stdint.h>

#define I2C_SDA_Pin   GPIO_PIN_8
#define I2C_SDA_Port  GPIOA
#define I2C_SCL_Pin   GPIO_PIN_12
#define I2C_SCL_Port  GPIOA

void i2cWrite1byte( uint8_t addr, uint8_t reg );
void i2cWrite2bytes( uint8_t addr, uint8_t reg, uint8_t value );
uint16_t i2cRead2bytes( uint8_t addr );

#endif /* SRC_I2C_BITBANG_H_ */
