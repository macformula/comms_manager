/*
 * adxl345.h
 *
 *  Created on: Dec 24, 2022
 *      Author: hadyi
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

/*---------------------- INCLUDES ----------------------*/
#include "main.h"
#include "spi.h"

/*---------------------- MACROS ----------------------*/
#define BUF_LEN (uint8_t)2

// Registers' Address
#define DEVID 					0x00
#define BW_RATE					0x2C
#define DATA_FORMAT 			0x31
#define FIFO_CTL 				0x38
// DATA(X,Y,Z)0 holds the 8 LSB of acceleration data, DATA(X,Y,Z)1 holds the 2 MSB of acceleration data
// Ex. (1 represents where data can be) DATAX0 --> 11111111
// DATAX1 --> 00000011
#define DATAX0					0x32
#define DATAX1					0x33
#define DATAY0					0x34
#define DATAY1					0x35
#define DATAZ0					0x36
#define DATAZ1					0x37
#define POWER_CTL 				0x2D
#define THRESH_TAP				0x1D
#define DUR						0x21
#define TAP_AXES				0x2A
#define INT_ENABLE				0x2E
#define INT_MAP					0x2F
#define LATENT					0x22
#define WINDOW					0x23
#define THRESH_ACT				0x24
#define THRESH_INACT			0x25
#define TIME_INAT				0x26
#define ACT_INACT_CTL			0x27
#define THRESH_FF 				0x28
#define TIME_FF					0x29
#define OFFX					0x1E
#define OFFY					0x1F
#define OFFZ					0x20
#define INT_SOURCE				0x30

// Distinguishes between reading and writing to registers
#define READ					0x80
#define WRITE					0x00

// Magic numbers
#define DEVID_RETURN			0xE5
// Scale factor from Bits to G's
#define SCALE_FACTOR			(double)3.9

// Earth's Gravity in m/s^2
#define GRAVITY					(double)9.81

// Conversion formulas
#define G_TO_METERS(data, scale_factor) (data * GRAVITY)
#define G_TO_BITS(data, scale_factor) (data / scale_factor * 1000)
#define METERS_TO_G(data, scale_factor) (data / 9.81)
#define METERS_TO_BITS(data, scale_factor) (data / scale_factor * 1000 / GRAVITY)
#define BITS_TO_G(data, scale_factor) (data * (scale_factor/1000))
#define BITS_TO_METERS(data, scale_factor) (data * (scale_factor/1000) * GRAVITY)

/*---------------------- INIT DEFINITIONS ----------------------*/

// Enums for the POWER_CTL Register setup
typedef enum {
	AUTOSLEEPOFF = 0,
	AUTOSLEEPON = 1,
}TeADXL_Auto_Sleep;

typedef enum {
	LINKMODEOFF = 0,
	LINKMODEON = 1,
}TeADXL_Link_Mode;

typedef enum {
	STANDBY_MODE = 0,
	MEASUREMENT_MODE = 1,
}TeADXL_Measure_Mode;

typedef enum {
	NORMAL_MODE = 0,
	SLEEP_MODE = 1,
}TeADXL_Sleep_Mode;

typedef enum {
	SLEEP_RATE_8HZ = 0,
	SLEEP_RATE_4HZ = 1,
	SLEEP_RATE_2HZ = 2,
	SLEEP_RATE_1HZ = 3,
}TeADXL_Sleep_Rate;

// Enums for the DATA_FORMAT Register setup
typedef enum {
	SPIMODE_4WIRE = 0,
	SPIMODE_3WIRE = 1,
}TeADXL_SPI_Mode;


typedef enum {
	INT_ACTIVEHIGH = 0,
	INT_ACTIVELOW = 1,
}TeADXL_Int_Mode;

typedef enum {
	RESOLUTION_10BIT = 0,
	RESOLUTION_FULL = 1,
}TeADXL_Resolution;

typedef enum {
	JUSTIFY_RIGHT = 0,
	JUSTIFY_LEFT = 1,
}TeADXL_Justify;

typedef enum {
	RANGE_2G = 0,
	RANGE_4G = 1,
	RANGE_8G = 2,
	RANGE_16G = 3,
}TeADXL_Range;

// Enums for BW_Rate register
typedef enum {
	LPMODE_NORMAL = 0,
	LPMODE_LOWPOWER = 1,
}TeADXL_Low_Power_Mode;

// In Low Power mode, you can only choose from 7 to 12
typedef enum {
	BWRATE_0_10 = 0,
	BWRATE_0_20 = 1,
	BWRATE_0_39 = 2,
	BWRATE_0_78 = 3,
	BWRATE_1_56 = 4,
	BWRATE_3_13 = 5,
	BWRATE_6_25 = 6,
	BWRATE_12_5 = 7,
	BWRATE_25 = 8,
	BWRATE_50 = 9,
	BWRATE_100 = 10,
	BWRATE_200 = 11,
	BWRATE_400 = 12,
	BWRATE_800 = 13,
	BWRATE_1600 = 14,
	BWRATE_3200 = 15,
}TeADXL_BW_Rate;


/*---------------------- DEFINITIONS ----------------------*/

// Distinguishes between units of the acceleration data
typedef enum {
	G = 0,
	BITS,
	METERS,
}TeADXL_Unit;

/*------------- STRUCT DEFINITIONS ------------- */
// ADXL345 struct holds the x, y, z acceleration data
typedef struct {
	double x;
	double y;
	double z;
}TsADXL_Data;

typedef struct {
	// SPI Instance
	TsSPI* spi;
	// Data Struct for accel values
	TsADXL_Data* data;
	// For POWER_CTL
	TeADXL_Auto_Sleep AutoSleep;
	TeADXL_Link_Mode LinkMode;
	TeADXL_Measure_Mode MeasureMode;
	TeADXL_Sleep_Mode SleepMode;
	TeADXL_Sleep_Rate SleepRate;
	// FOR DATA_FORMAT
	TeADXL_SPI_Mode SPIMode;
	TeADXL_Int_Mode IntMode;
	TeADXL_Resolution Resolution;
	TeADXL_Justify Justify;
	TeADXL_Range Range;
	// BW_RATE STUFF
	TeADXL_Low_Power_Mode LPMode;
	TeADXL_BW_Rate Rate;
}TsADXL_InitTypeDef;

// TeADXL_Status describes the return types for all ADXL functions
typedef enum {
	ADXL_OK = 0,
	ADXL_FAILED,
	ADXL_NULL
}TeADXL_Status;


/*------------- PUBLIC FUNCTION DEFINITIONS ------------- */
// Returns the acceleration as m/s^2 from 1 direction (either x, y, or z)
uint16_t ADXL_Read_Direction(TsADXL_InitTypeDef* adxl, uint8_t lsb_reg, uint8_t msb_reg);

// passes in data in any format and returns it in the specified return format,
// either g's, m/s^2 or as raw data (bits)
double Format_Accel(TsADXL_InitTypeDef* adxl, double data, TeADXL_Unit incoming_units, TeADXL_Unit outgoing_units);

// populates the ADXL345_st struct with the current x, y, z acceleration data in m/s^2
TeADXL_Status ADXL_Get_Accel(TsADXL_InitTypeDef* adxl);

// Initialize the ADXL345 by writing to the power register and data format register
TeADXL_Status ADXL_Init(TsADXL_InitTypeDef* adxl);

// returns 1 if DEVID returns 0xE5, else 0
TeADXL_Status Check_DEVID_Register(TsADXL_InitTypeDef* adxl);

#endif /* INC_ADXL345_H_ */
