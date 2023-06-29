/*
 * spi.c
 *
 *  Created on: Nov 11, 2022
 *      Author: Hady I.
 */

/*---------------------- INCLUDES ----------------------*/
#include "spi.h"

/*---------------------- MACROS ----------------------*/
#define TIMEOUT (uint8_t)100

/*---------------------- PRIVATE FUNCTIONS ----------------------*/
// SPI_Select configures the corresponding SPI number from a SPI_st
static TeSPI_Status SPI_Select(TsSPI* spi)
{
	// Switch case for selection of 6 SPIS
	switch(spi->spi_num)
	{
		case 1:
			spi->hspi-> Instance = SPI1;
			break;
		case 2:
			spi->hspi-> Instance = SPI2;
			break;
		case 3:
			spi->hspi-> Instance = SPI3;
			break;
		case 4:
			spi->hspi-> Instance = SPI4;
			break;
		case 5:
			spi->hspi-> Instance = SPI5;
			break;
		case 6:
			spi->hspi-> Instance = SPI6;
			break;
		default:
			return INVALID_SPI_NUM;
	}

	return SPI_OK;
}

// SPI_Baud_Rate_Select configures the baud rate from the one specified in baudrate
static TeSPI_Status SPI_Baud_Rate_Select(TsSPI* spi)
{
	// Baud rate must be positive
	uint32_t quotient = HAL_RCC_GetSysClockFreq()/(spi->baudrate*1000);

	if (quotient <=2)			spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	else if (quotient <=4)		spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	else if (quotient <=8)		spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	else if (quotient <=16)		spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	else if (quotient <=32)		spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	else if (quotient <=64)		spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	else if (quotient <=128) 	spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	else if (quotient > 128)	spi->hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	else						return SPI_BAUDRATE_OUT_OF_BOUNDS;

	return SPI_OK;
}

// SPI_Datasize_Select configures the data size from a TeSPI_Datasize
static TeSPI_Status SPI_Datasize_Select(TsSPI* spi)
{
	spi->hspi -> Init.DataSize = spi->datasize;
	return SPI_OK;
}

// SPI_Clock_Phase_Select configures the clock phase based on a TeSPI_Clock_Phase
static TeSPI_Status SPI_Clock_Phase_Select(TsSPI* spi)
{
	switch(spi->clock_phase)
	{
	case(EDGE_0):
		spi->hspi -> Init.CLKPhase = SPI_PHASE_1EDGE;
		break;
	case(EDGE_1):
		spi->hspi -> Init.CLKPhase = SPI_PHASE_2EDGE;
		break;
	default:
		return SPI_INVALID_CLOCK_PHASE;
	}

	return SPI_OK;
}

// SPI_Clock_Polarity_Select configures the clock polarity based on a TeSPI_Clock_Polarity
static TeSPI_Status SPI_Clock_Polarity_Select(TsSPI* spi)
{
	switch(spi->clock_polarity)
	{
	case(LOW):
		spi->hspi -> Init.CLKPolarity = SPI_POLARITY_LOW;
		break;
	case(HIGH):
		spi->hspi -> Init.CLKPolarity = SPI_POLARITY_HIGH;
		break;
	default:
		return SPI_INVALID_CLOCK_POLARITY;
	}

	return SPI_OK;
}

// SPI_MSB_Select configures the bit position based on a TeSPI_Bit_Order
static TeSPI_Status SPI_MSB_Select(TsSPI* spi)
{
	switch(spi->bit_order){
		case LSB_FIRST:
			spi->hspi -> Init.FirstBit = SPI_FIRSTBIT_LSB;
			break;
		case MSB_FIRST:
			spi->hspi -> Init.FirstBit = SPI_FIRSTBIT_MSB;
			break;
		default:
			return SPI_INVALID_BIT_ORDER;
	}

	return SPI_OK;
}

// Current configurations that are not being modified
static void SPI_Default_Configs(TsSPI* spi)
{
	spi->hspi -> Init.Mode = SPI_MODE_MASTER;		// TODO: look into SLAVE version of this
	spi->hspi -> Init.Direction = SPI_DIRECTION_2LINES;
	spi->hspi -> Init.NSS = SPI_NSS_SOFT;
	spi->hspi -> Init.TIMode = SPI_TIMODE_DISABLE;
	spi->hspi -> Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi->hspi -> Init.CRCPolynomial = 7;
	spi->hspi -> Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	spi->hspi -> Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
}

/*------------- PUBLIC FUNCTION DEFINITIONS ------------- */

TeSPI_Status SPI_Init(TsSPI* spi)
{
	TeSPI_Status response;
	SPI_Default_Configs(spi);

	response = SPI_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	response = SPI_Baud_Rate_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	response = SPI_Datasize_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	response = SPI_Clock_Phase_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	response = SPI_Clock_Polarity_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	response = SPI_MSB_Select(spi);
	if (response != SPI_OK) {
		return response;
	}

	if (HAL_SPI_Init(spi->hspi) != HAL_OK) { return SPI_INIT_FAILED; }

	// Set cs_pin high (tells slaves to ignore info)
    HAL_GPIO_WritePin(spi->cs_port, spi->pin, GPIO_PIN_SET);

	return SPI_OK;
}

TeSPI_Status SPI_Transmit(TsSPI* spi, uint8_t *tx_buf, uint8_t buf_len)
{
	HAL_StatusTypeDef tx_response;

	HAL_GPIO_WritePin(spi->cs_port, spi->pin, GPIO_PIN_RESET);
	tx_response = HAL_SPI_Transmit(spi->hspi, (uint8_t *)tx_buf, buf_len, TIMEOUT);
	HAL_GPIO_WritePin(spi->cs_port, spi->pin, GPIO_PIN_SET);
	if (tx_response != HAL_OK) {
		return SPI_TRANSMIT_FAILED;
	}
	return SPI_OK;
}

TeSPI_Status SPI_Transmit_Receive(TsSPI* spi, uint8_t *tx_buf, uint8_t *rx_buf, uint8_t buf_len)
{
	HAL_StatusTypeDef rx_response;

	HAL_GPIO_WritePin(spi->cs_port, spi->pin, GPIO_PIN_RESET);
	rx_response = HAL_SPI_TransmitReceive(spi->hspi, (uint8_t *)tx_buf, (uint8_t *)rx_buf, buf_len, TIMEOUT);
	HAL_GPIO_WritePin(spi->cs_port, spi->pin, GPIO_PIN_SET);
	if (rx_response != HAL_OK) {
		return SPI_RECEIVE_FAILED;
	}
	return SPI_OK;
}

TeSPI_Status SPI_Deinit(TsSPI* spi)
{
	HAL_StatusTypeDef deinit_response;

	deinit_response = HAL_SPI_DeInit(spi->hspi);
	if (deinit_response != HAL_OK) {
		return SPI_DEINIT_FAILED;
	}

	return SPI_OK;
}
