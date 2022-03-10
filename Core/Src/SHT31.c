/*
 * SHT31.c
 *
 *  Created on: Sep 22, 2021
 *      Author: fhdtr trung pham
 */
#include "SHT31.h"

uint16_t MeasureSingleShotCMD[2][3] = {
		{SHT31_MEASUREMENT_STRETCH_HIGH, SHT31_MEASUREMENT_STRETCH_MEDIUM, SHT31_MEASUREMENT_STRETCH_LOW},
		{SHT31_MEASUREMENT_NOSTRETCH_HIGH, SHT31_MEASUREMENT_NOSTRETCH_MEDIUM, SHT31_MEASUREMENT_NOSTRETCH_LOW}
};
uint16_t MeasurePeriodicCMD[5][3] = {
		{SHT31_MEASUREMENT_PERIODIC_05_HIGH, SHT31_MEASUREMENT_PERIODIC_05_MEDIUM, SHT31_MEASUREMENT_PERIODIC_05_LOW},
		{SHT31_MEASUREMENT_PERIODIC_1_HIGH, SHT31_MEASUREMENT_PERIODIC_1_MEDIUM, SHT31_MEASUREMENT_PERIODIC_1_LOW},
		{SHT31_MEASUREMENT_PERIODIC_2_HIGH, SHT31_MEASUREMENT_PERIODIC_2_MEDIUM, SHT31_MEASUREMENT_PERIODIC_2_LOW},
		{SHT31_MEASUREMENT_PERIODIC_4_HIGH, SHT31_MEASUREMENT_PERIODIC_4_MEDIUM, SHT31_MEASUREMENT_PERIODIC_4_LOW},
		{SHT31_MEASUREMENT_PERIODIC_10_HIGH, SHT31_MEASUREMENT_PERIODIC_10_MEDIUM, SHT31_MEASUREMENT_PERIODIC_10_LOW}
};
uint16_t MilsPerMeasure[5] = {2005, 1005, 505, 255, 105}; // table 9 page 11 in column mps, + 5ms for correctly read
//                           0.5     1    2    4   10
uint8_t MeasureTime[3] = {13,5,3}; // table 4 page 7

void SHT31_Config(uint8_t address, I2C_HandleTypeDef* hi2c) {
	sht31.address = address;
	sht31.hi2c = hi2c;

	sht31.mode = SHT31_SingleShot;
	sht31.isCurrentlyPeriodic = 0;

	sht31.lastMeasureTime = HAL_GetTick();
}

SHT31_Status SHT31_SendCommand(uint16_t command) {
	uint8_t buffer[2] = {command >> 8, command & (0xFF)};
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(sht31.hi2c, sht31.address<<1, buffer, 2, HAL_MAX_DELAY);
	if(ret == HAL_OK) {
		return SHT31_OK;
	}
	return SHT31_Transmit_Error;
}

SHT31_Status SHT31_GetBytes(uint8_t* buffer, int num) {
	HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(sht31.hi2c, sht31.address<<1, buffer, num, HAL_MAX_DELAY);
	if(ret == HAL_OK) {
		return SHT31_OK;
	}
	return SHT31_Transmit_Error;
}

SHT31_Status SHT31_GetData(Mode mode, Repeatability rep, ClockStretch stretch,MPS mps) {
	uint16_t command = SHT31_GetCommand(mode, rep, stretch, mps);

	SHT31_Status ret;

	if(sht31.mode == SHT31_SingleShot) {

		ret = SHT31_SendCommand(command);
		if(ret != SHT31_OK) {
			return SHT31_Transmit_Error;
		}

	}
	else if(sht31.mode == SHT31_Periodic){
		if(HAL_GetTick() - sht31.lastMeasureTime > MilsPerMeasure[mps]) { // must wait for enough time before read in Periodic mode
			sht31.lastMeasureTime = HAL_GetTick();

			if(sht31.isCurrentlyPeriodic == 0) { // Just the first period in Periodic mode need to send command

				ret = SHT31_SendCommand(command);
				if(ret != SHT31_OK) {
					return SHT31_Transmit_Error;
				}

				sht31.isCurrentlyPeriodic = 1;
			}
		}
		else {
			return SHT31_Not_Ready;
		}
	}

	HAL_Delay(MeasureTime[rep]); // measure duration in table 4 page 7
	uint8_t buffer[6];
	SHT31_GetBytes(buffer, 6);



	if(buffer[2] != SHT31_CRC_8(&buffer[0], 2)) {
		ret =  SHT31_Data_Error;
	}
	else {
		sht31.temperature_raw = (buffer[0] << 8) + buffer[1];
		ret = SHT31_OK;
	}

	if(buffer[5] != SHT31_CRC_8(&buffer[3], 2)) {
		ret =  SHT31_Data_Error;
	}
	else {
		sht31.humidity_raw = (buffer[3] << 8) + buffer[4];
		ret = SHT31_OK;
	}
	return ret;



}

uint16_t SHT31_GetCommand(Mode mode, Repeatability rep, ClockStretch stretch,MPS mps) {
	if(mode == SHT31_SingleShot) {
		sht31.mode = SHT31_SingleShot;
		return MeasureSingleShotCMD[stretch][rep];
	}
	else if(mode == SHT31_Periodic) {
		sht31.mode = SHT31_Periodic;

		return MeasurePeriodicCMD[mps][rep];
	}
	return 0x0;
}


uint8_t SHT31_CRC_8(uint8_t* data, int len) {
	uint8_t crc = 0xFF;
	const uint8_t poly = 0x31;

	for(uint8_t byte = len; byte; byte--) {
		crc ^= *(data++);
		for(uint8_t i = 8; i; i--) {
			crc = (crc & 0x80)? (crc<<1)^poly : (crc<<1);
		}
	}
	return crc;
}



float SHT31_GetHumidity() {
	return 100.0*(sht31.humidity_raw)/65535;
}
float SHT31_GetTemperature() {
	return 175.0*(sht31.temperature_raw)/65535 - 45;
}

uint16_t SHT31_GetHumidity_raw() {
	return sht31.humidity_raw;
}
uint16_t SHT31_GetTemperature_raw() {
	return sht31.temperature_raw;
}

