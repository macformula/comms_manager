/*
 * ADXL345.c
 *
 *  Created on: Dec 24, 2022
 *      Author: hadyi
 */

/*---------------------- INCLUDES ----------------------*/
#include "ADXL345.h"
#include <math.h>

/*---------------------- MACROS ----------------------*/
#define BUF_LEN (uint8_t)2

/*---------------------- HELPER FUNCTIONS ----------------------*/
// Takes an address and formats it based on if you are reading or writing to it
/* @param read_write - takes in READ or WRITE macro */
static uint8_t Format_Register(uint8_t address, uint8_t read_write) {
	return address | read_write;
}

/*------------- PRIVATE FUNCTION DEFINITIONS ------------- */
// Reads a register and returns the 8-bit value given the address and SPI struct
static uint8_t Read_Register(TsSPI* spi, uint8_t address) {
	uint8_t tx_buf[2] = {0x00};
	tx_buf[0] = Format_Register(address, READ);
	tx_buf[1] = 0x00;

	uint8_t rx_buf[2] = {0x00};
	SPI_Transmit_Receive(spi, tx_buf, rx_buf, BUF_LEN);
	return rx_buf[1];
}

// Writes an 8-bit value to a register given the address and SPI struct
static TeADXL_Status Write_Register(TsSPI* spi, uint8_t address, uint8_t data) {
	uint8_t tx_buf[2] = {0x00};
	tx_buf[0] = Format_Register(address, WRITE);
	tx_buf[1] = data;
	TeSPI_Status response = SPI_Transmit(spi, tx_buf, BUF_LEN);
	if (response != SPI_OK) return ADXL_FAILED;
	return ADXL_OK;
}

static TeADXL_Status ADXL_Power_Ctl_Init(TsADXL_InitTypeDef* adxl) {
	uint8_t formatreg = 0x00;
	formatreg = (adxl->LinkMode << 5) | (adxl->AutoSleep << 4) | (adxl->MeasureMode << 3) | (adxl->SleepMode << 2);
	formatreg += (adxl -> SleepRate);
	TeADXL_Status response = Write_Register(adxl->spi, POWER_CTL, formatreg);
	return response;
}

static TeADXL_Status ADXL_Data_Format_Init(TsADXL_InitTypeDef* adxl) {
	uint8_t formatreg = 0x00;
	formatreg = (adxl->SPIMode << 6) | (adxl->IntMode << 5) | (adxl->Resolution << 3) | (adxl->Justify << 2);
	formatreg += (adxl -> Range);
	TeADXL_Status response = Write_Register(adxl->spi, DATA_FORMAT, formatreg);
	return response;
}

static TeADXL_Status ADXL_BW_Rate_Init(TsADXL_InitTypeDef* adxl) {
	uint8_t formatreg = 0x00;
	formatreg = (adxl->LPMode << 4);
	if (adxl->LPMode == LPMODE_LOWPOWER) {
		if ((adxl->Rate) > 12) formatreg += 12;
		else if ((adxl->Rate) < 7) formatreg += 7;
	} else formatreg += (adxl->Rate);
	TeADXL_Status response = Write_Register(adxl->spi, BW_RATE, formatreg);
	return response;
}

/*------------- PUBLIC FUNCTION DEFINITIONS ------------- */
// Returns the acceleration as m/s^2 from 1 direction (either x, y, or z)
uint16_t ADXL_Read_Direction(TsADXL_InitTypeDef* adxl, uint8_t lsb_reg, uint8_t msb_reg) {
	uint16_t lsb_data = Read_Register(adxl->spi, lsb_reg);
	lsb_data &= 0x00FF;

	uint16_t msb_data = Read_Register(adxl->spi, msb_reg);
	msb_data <<= 8;
	msb_data &= 0x0300;
	uint16_t total = msb_data | lsb_data;
	return total;
}

// passes in data in any format and returns it in the specified return format,
// either g's, m/s^2 or as raw data (bits)
// 3.9 is the scale factor for +- 2g's.
double Format_Accel(TsADXL_InitTypeDef* adxl, double data, TeADXL_Unit incoming_units, TeADXL_Unit outgoing_units) {
	double scale_factor = pow(2,(adxl->Range)) * SCALE_FACTOR;
	switch(incoming_units)
	{
		case G:
			switch(outgoing_units)
			{
				case METERS: return G_TO_METERS(data, scale_factor);
				case BITS: return G_TO_BITS(data, scale_factor);
				default: return data;
			}
			break;
		case METERS:
			switch(outgoing_units)
			{
				case G: return METERS_TO_G(data, scale_factor);
				case BITS: return METERS_TO_BITS(data, scale_factor);
				default: return data;
			}
			break;
		case BITS:
			switch(outgoing_units)
			{
				case G: return BITS_TO_G(data, scale_factor);
				case METERS: return BITS_TO_METERS(data, scale_factor);
				default: return data;
			}
			break;
		default: return data;
	}
}

// populates the ADXL345_st struct with the current x, y, z acceleration data in m/s^2
TeADXL_Status ADXL_Get_Accel(TsADXL_InitTypeDef* adxl) {
	if (adxl == NULL || adxl->spi == NULL) return ADXL_FAILED;
	uint16_t x = ADXL_Read_Direction(adxl, DATAX0, DATAX1);
	uint16_t y = ADXL_Read_Direction(adxl, DATAY0, DATAY1);
	uint16_t z = ADXL_Read_Direction(adxl, DATAZ0, DATAZ1);
	adxl->data->x = Format_Accel(adxl, x, BITS, METERS);
	adxl->data->y = Format_Accel(adxl, y, BITS, METERS);
	adxl->data->z = Format_Accel(adxl, z, BITS, METERS);
	return ADXL_OK;
}

// Initialize the ADXL345 by writing to the power register and data format register
// For our use, write 0x08 to both registers
TeADXL_Status ADXL_Init(TsADXL_InitTypeDef* adxl) {
	TeADXL_Status response;

	response = ADXL_Power_Ctl_Init(adxl);
	if (response != ADXL_OK) {
		return response;
	}

	response = ADXL_Data_Format_Init(adxl);
	if (response != ADXL_OK) {
		return response;
	}

	response = ADXL_BW_Rate_Init(adxl);
	if (response != ADXL_OK) {
		return response;
	}

	return ADXL_OK;
}

// returns ADXL_OK if DEVID returns 0xE5, else ADXL_FAILED
TeADXL_Status Check_DEVID_Register(TsADXL_InitTypeDef* adxl) {
	return Read_Register(adxl->spi, DEVID) == DEVID_RETURN ? ADXL_OK : ADXL_FAILED;
}
