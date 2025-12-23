/*
 * i2c_bitbang.c
 *
 *  Created on: Dec 13, 2025
 *      Author: m.lempertseder
 * 
 * Licensed under the MIT License.
 * See LICENSE and file headers for details.
 */

#ifndef SRC_I2C_BITBANG_C_
#define SRC_I2C_BITBANG_C_

#include "i2c_bitbang.h"

uint8_t getSDApin( void ){
	return (uint8_t)HAL_GPIO_ReadPin(I2C_SDA_Port, I2C_SDA_Pin);
}

void SDAlow( void ){
	HAL_GPIO_WritePin(I2C_SDA_Port, I2C_SDA_Pin, GPIO_PIN_RESET);
}

void SDAhigh( void ){
	HAL_GPIO_WritePin(I2C_SDA_Port, I2C_SDA_Pin, GPIO_PIN_SET);
}

void SCLlow( void ){
	HAL_GPIO_WritePin(I2C_SCL_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
}

void SCLhigh( void ){
	HAL_GPIO_WritePin(I2C_SCL_Port, I2C_SCL_Pin, GPIO_PIN_SET);
}

void wStart( void ){
	SDAhigh();
	SCLhigh();
	SDAlow(); // start
}

void wStop( void ){
	SCLlow();
	SDAlow();
	SCLhigh();
	SDAhigh(); // stop
}

void w8bits( uint8_t val ){
	for (int i=0;i<8;i++)
	{
		SCLlow();
		if (val & 0x80)
			SDAhigh();
		else
			SDAlow();
		SCLhigh();
		val = val << 1;
	}

	// ack by client
	SCLlow();
	SDAhigh();
	SCLhigh();
}

uint8_t r8bits( uint8_t noAck ){
	uint8_t readval = 0;

	for (int i=0;i<8;i++)
	{
		SCLlow();
		if (i==0)
			SDAhigh(); // release ack
		SCLhigh();
		if (getSDApin())
			readval |= 0x01;
		if (i<7)
			readval = readval << 1;
	}

	SCLlow();
	if (noAck)
		SDAhigh();
	else
		SDAlow(); // ack by master
	SCLhigh();
	return readval;
}

void i2cWrite1byte( uint8_t addr, uint8_t reg ) {
	addr = addr << 1;
	wStart();
	w8bits( addr ); // addr & r/w bit
	w8bits( reg );
	wStop();
}

void i2cWrite2bytes( uint8_t addr, uint8_t reg, uint8_t value ) {
	addr = addr << 1;
	wStart();
	w8bits( addr ); // addr & r/w bit
	w8bits( reg );
	w8bits( value );
	wStop();
}

uint16_t i2cRead2bytes( uint8_t addr ) {
	addr = addr << 1;
	addr |= 0x01; // r/w bit high -> read
	uint16_t readval = 0;

	wStart();
	w8bits( addr ); // addr & r/w bit
	readval = r8bits( 0 ); // high byte
	readval = (readval<<8) + r8bits( 1 ); // low byte
	wStop();

	return readval;
}

#endif /* SRC_I2C_BITBANG_C_ */
