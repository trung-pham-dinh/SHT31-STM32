/*
 * SHT31.h
 *
 *  Created on: Sep 22, 2021
 *      Author: fhdtr trung pham
 */

#ifndef INC_SHT31_H_
#define INC_SHT31_H_

#include "main.h"

// Address
#define SHT31_ADDRESS_A 0x44 // table 7 page 9
#define SHT31_ADDRESS_B 0x45 // shift left 1 in order to leave room for R/W bit

// MEASUREMENT Single shot
#define SHT31_MEASUREMENT_STRETCH_HIGH 0x2C06 // table 8 page 10
#define SHT31_MEASUREMENT_STRETCH_MEDIUM 0x2C0D
#define SHT31_MEASUREMENT_STRETCH_LOW 0x2C10

#define SHT31_MEASUREMENT_NOSTRETCH_HIGH 0x2400
#define SHT31_MEASUREMENT_NOSTRETCH_MEDIUM 0x240B
#define SHT31_MEASUREMENT_NOSTRETCH_LOW 0x2416

// MEASUREMENT Periodic
#define SHT31_MEASUREMENT_PERIODIC_05_HIGH 0x2032 // table 9 page 11
#define SHT31_MEASUREMENT_PERIODIC_05_MEDIUM 0x2024
#define SHT31_MEASUREMENT_PERIODIC_05_LOW 0x202F

#define SHT31_MEASUREMENT_PERIODIC_1_HIGH 0x2130
#define SHT31_MEASUREMENT_PERIODIC_1_MEDIUM 0x2126
#define SHT31_MEASUREMENT_PERIODIC_1_LOW 0x212D

#define SHT31_MEASUREMENT_PERIODIC_2_HIGH 0x2236
#define SHT31_MEASUREMENT_PERIODIC_2_MEDIUM 0x2220
#define SHT31_MEASUREMENT_PERIODIC_2_LOW 0x222B

#define SHT31_MEASUREMENT_PERIODIC_4_HIGH 0x2334
#define SHT31_MEASUREMENT_PERIODIC_4_MEDIUM 0x2322
#define SHT31_MEASUREMENT_PERIODIC_4_LOW 0x2329

#define SHT31_MEASUREMENT_PERIODIC_10_HIGH 0x2737
#define SHT31_MEASUREMENT_PERIODIC_10_MEDIUM 0x2721
#define SHT31_MEASUREMENT_PERIODIC_10_LOW 0x272A

#define SHT31_PERIODIC_FETCH_DATA 0xE000

typedef enum {
	SHT31_SingleShot,
	SHT31_Periodic
} Mode;

typedef enum {
	SHT31_High,
	SHT31_Medium,
	SHT31_Low
}Repeatability;

typedef enum {
	SHT31_05,
	SHT31_1,
	SHT31_2,
	SHT31_4,
	SHT31_10,
	SHT31_NON_MPS
}MPS;

typedef enum {
	SHT31_En_Stretch,
	SHT31_Dis_Stretch,
	SHT31_NON_Stretch
}ClockStretch;

typedef enum {
	SHT31_OK,
	SHT31_Data_Error,
	SHT31_Timeout,
	SHT31_Not_Ready,
	SHT31_Transmit_Error,
	SHT31_Receive_Error
} SHT31_Status;


struct SHT31_INFO{
	uint8_t address;
	I2C_HandleTypeDef* hi2c;

	Mode mode;
	int isCurrentlyPeriodic;

	long long lastMeasureTime;

	uint16_t temperature_raw;
	uint16_t humidity_raw;
	float temperature;
	float humidity;
}sht31;


SHT31_Status SHT31_SendCommand(uint16_t);
SHT31_Status SHT31_GetBytes(uint8_t*, int);
uint8_t SHT31_CRC_8(uint8_t*, int);
uint16_t SHT31_GetCommand(Mode, Repeatability, ClockStretch, MPS);


void SHT31_Config(uint8_t, I2C_HandleTypeDef*);
SHT31_Status SHT31_GetData(Mode, Repeatability, ClockStretch, MPS);

float SHT31_GetHumidity();
float SHT31_GetTemperature();
uint16_t SHT31_GetHumidity_raw();
uint16_t SHT31_GetTemperature_raw();


//

#endif /* INC_SHT31_H_ */
