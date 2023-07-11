/*
 * spi_lib.h
 *
 *  Created on: Nov 11, 2022
 *      Author: Hady I.
 */

#ifndef INC_SPI_LIB_H_
#define INC_SPI_LIB_H_

/*---------------------- INCLUDES ----------------------*/
#include "main.h"

/*---------------------- DEFINITIONS ----------------------*/
// Distinguishes between 0 and 1 (sometimes 1 and 2) edge clock phase
typedef enum {
	EDGE_0 = 0,
	EDGE_1
}TeSPI_Clock_Phase;

// Distinguishes between high and low clock polarity
typedef enum {
	LOW = 0,
	HIGH
}TeSPI_Clock_Polarity;

// Distinguishes between the bit position of the data stream
typedef enum {
	LSB_FIRST = 0,	// Send least significant bit first
	MSB_FIRST		// Send most significant bit first
}TeSPI_Bit_Order;

// Distinguishes between allowable data bits to send at a time (4-16 bits)
typedef enum {
	SPI_DATASIZE_4 = SPI_DATASIZE_4BIT,
	SPI_DATASIZE_5 = SPI_DATASIZE_5BIT,
	SPI_DATASIZE_6 = SPI_DATASIZE_6BIT,
	SPI_DATASIZE_7 = SPI_DATASIZE_7BIT,
	SPI_DATASIZE_8 = SPI_DATASIZE_8BIT,
	SPI_DATASIZE_9 = SPI_DATASIZE_9BIT,
	SPI_DATASIZE_10 = SPI_DATASIZE_10BIT,
	SPI_DATASIZE_11 = SPI_DATASIZE_11BIT,
	SPI_DATASIZE_12 = SPI_DATASIZE_12BIT,
	SPI_DATASIZE_13 = SPI_DATASIZE_13BIT,
	SPI_DATASIZE_14 = SPI_DATASIZE_14BIT,
	SPI_DATASIZE_15 = SPI_DATASIZE_15BIT,
	SPI_DATASIZE_16 = SPI_DATASIZE_16BIT
}TeSPI_Datasize;

// TsSPI holds the user input for a particular SPI configuration
typedef struct {
	// pointer to the SPI handle being used
	SPI_HandleTypeDef* hspi;
	// SPI baud rate (must be > 0)
	uint32_t baudrate;
	// SPI Chip Select Port, this is defined upon definition of struct
	GPIO_TypeDef* cs_port;
	// SPI Pin, this is defined upon definition of struct
	uint16_t pin;
	// SPI data frame size, from 4-16
	TeSPI_Datasize datasize;
	// SPI clock phase being selected, choose between 0 and 1 edge
	TeSPI_Clock_Phase clock_phase;
	// SPI clock polarity being selected, choose between high and low
	TeSPI_Clock_Polarity clock_polarity;
	// Selection for the first or second bit to be sent first
	TeSPI_Bit_Order bit_order;
	// SPI number, from 1-6
	uint8_t spi_num;
}TsSPI;

// TeSPI_Status describes the return types for all SPI functions
typedef enum {
	SPI_OK = 0,
	SPI_BAUDRATE_OUT_OF_BOUNDS,
	SPI_INVALID_CLOCK_PHASE,
	SPI_INVALID_CLOCK_POLARITY,
	SPI_INVALID_BIT_ORDER,
	INVALID_SPI_NUM,
	SPI_INVALID_DATASIZE,
	SPI_TRANSMIT_FAILED,
	SPI_RECEIVE_FAILED,
	SPI_DEINIT_FAILED,
	SPI_INIT_FAILED
}TeSPI_Status;

/*------------- PUBLIC FUNCTION DEFINITIONS ------------- */

TeSPI_Status SPI_Init(TsSPI* spi);
TeSPI_Status SPI_Transmit(TsSPI* spi, uint8_t *Tx_buf, uint8_t buf_len);
TeSPI_Status SPI_Deinit(TsSPI* spi);
TeSPI_Status SPI_Transmit_Receive(TsSPI* spi, uint8_t *tx_buf, uint8_t *rx_buf, uint8_t buf_len);


#endif /* INC_SPI_LIB_H_ */
