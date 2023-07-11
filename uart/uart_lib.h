/*
 * uart_lib.h
 *
 *  Created on: Oct 26, 2022
 *      Author: ivanl
 */

#ifndef INC_UART_LIB_H_
#define INC_UART_LIB_H_


/*---------------------- INCLUDES ----------------------*/

#include "main.h"

/*---------------------- MACROS ----------------------*/

#define MIN_UART_BAUDRATE (123U)
#define MAX_UART_BAUDRATE (500000U)
#define TIMEOUT 		  (5000U)

/*---------------------- TYPEDEFS ----------------------*/

// Distinguishes between 3 UART modes
typedef enum {
	UART_RX,	// Receive only
	UART_TX,		// Transmit only
	UART_TX_RX		// Receive and transmit
}TeUART_Mode;

// Distinguishes between the bit position of the data stream
typedef enum {
	LSB_First,	// Send least significant bit first
	MSB_First		// Send most significant bit first
}TeUART_Bit_Position;

// Distinguishes between allowable data bits to send at a time (7-9 bits)
typedef enum {
    UART_Datasize_7 = ((uint8_t)7),
    UART_Datasize_8 = ((uint8_t)8),
    UART_Datasize_9 = ((uint8_t)9),
}TeUART_Datasize;

// Predetermines standard baudrates for UART
typedef enum {
    UART_9600 	=   ((uint32_t) 9600),
    UART_19200 	=  	((uint32_t) 19200),
	UART_57600 	= 	((uint32_t) 57600),
    UART_115200 =   ((uint32_t) 115200),
    UART_256000 =   ((uint32_t) 256000),
    UART_500000 =   ((uint32_t) 500000),
}TeUART_Std_Baud;

// UART_st holds the user input for a particular UART configuration
typedef struct {
	// pointer to the UART handle being used
	UART_HandleTypeDef* huart;
	// UART number, from 1-8
	uint8_t uart_num;
	// UART baud rate, from 123 Bits/s and 500 KBits/s
	TeUART_Std_Baud baudrate;
	// UART data frame size, from 7-9
	TeUART_Datasize datasize;
	// UART mode being selected, choose from Tx only, Rx only, or Tx and Rx
	TeUART_Mode mode;
	// Selection for the first or second bit to be sent first
	TeUART_Bit_Position bit_position;
}UART_st;

// UART_Return_et describes the return types for all UART functions
typedef enum {
	UART_UNKNOWN_STATUS,
	UART_OK,
	UART_BAUDRATE_OUT_OF_BOUNDS,
	UART_INVALID_MODE,
	UART_INVALID_BIT_POSITION,
	UART_INVALID_UART_NUM,
	UART_INVALID_DATASIZE,
	UART_TRANSMIT_FAILED,
	UART_RECEIVE_FAILED,
	UART_DEINIT_FAILED,
}TeUART_Return;

/*------------ PUBLIC FUNCTION DECLARATIONS ------------- */

TeUART_Return UART_Init(UART_st* uart);
TeUART_Return UART_Transmit(UART_st* uart, uint8_t* tx_buf, uint8_t buf_len);
TeUART_Return UART_Receive(UART_st* uart, uint8_t* rx_buf, uint8_t buf_len);
TeUART_Return UART_Deinit(UART_st* uart);

#endif /* INC_UART_H_ */
