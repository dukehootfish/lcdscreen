/*
 * File: MQ2.c
 * Driver Name: [[ MQ2 Smoke / Gas Sensor ]]
 * SW Layer:   ECUAL
 * Created on: Oct 20, 2026
 *
 */

#include "MQ2.h"

/*-----------------------[INTERNAL DEFINITIONS]-----------------------*/

#define MQ2_ADC_MAX             4095    // 12-bit ADC full scale
#define MQ2_ADC_TIMEOUT_MS      10

/*-----------------------[INTERNAL VARIABLES]-----------------------*/

static ADC_HandleTypeDef* MQ2_ADC_Handle_g;
static uint32_t MQ2_StartTick_g;
static uint32_t MQ2_Millivolts_g;
static MQ2_State_t MQ2_State_g = MQ2_WARMING_UP;

/*---------------------[STATIC INTERNAL FUNCTIONS]-----------------------*/

// Returns 1 and writes the averaged raw ADC value, or 0 if no conversion succeeded
static uint8_t MQ2_ReadAveragedRaw(uint32_t* Raw)
{
	uint32_t Sum = 0;
	uint32_t Count = 0;

	for (uint32_t i = 0; i < MQ2_SAMPLES; i++)
	{
		if (HAL_ADC_Start(MQ2_ADC_Handle_g) != HAL_OK)
		{
			continue;
		}
		if (HAL_ADC_PollForConversion(MQ2_ADC_Handle_g, MQ2_ADC_TIMEOUT_MS) == HAL_OK)
		{
			Sum += HAL_ADC_GetValue(MQ2_ADC_Handle_g);
			Count++;
		}
	}
	HAL_ADC_Stop(MQ2_ADC_Handle_g);

	if (Count == 0)
	{
		return 0;
	}
	*Raw = Sum / Count;
	return 1;
}

/*-----------------------------------------------------------------------*/

//=========================================================================================================================

/*-----------------------[USER EXTERNAL FUNCTIONS]-----------------------*/

// The ADC must already be initialized and calibrated
void MQ2_Init(ADC_HandleTypeDef* ADC_Handle)
{
	MQ2_ADC_Handle_g = ADC_Handle;
	MQ2_StartTick_g = HAL_GetTick();
	MQ2_Millivolts_g = 0;
	MQ2_State_g = MQ2_WARMING_UP;
}

// Takes a new reading and returns the updated sensor state
MQ2_State_t MQ2_Update(void)
{
	uint32_t Raw;

	if (MQ2_ReadAveragedRaw(&Raw))
	{
		uint32_t PinMv = (Raw * MQ2_VREF_MV) / MQ2_ADC_MAX;
		// Undo the voltage divider to get the voltage at the sensor's AO pin
		MQ2_Millivolts_g = (PinMv * (MQ2_DIVIDER_TOP_OHMS + MQ2_DIVIDER_BOTTOM_OHMS)) / MQ2_DIVIDER_BOTTOM_OHMS;
	}

	if (MQ2_GetWarmupSecondsLeft() > 0)
	{
		MQ2_State_g = MQ2_WARMING_UP;
	}
	else if (MQ2_Millivolts_g >= MQ2_ALARM_ON_MV)
	{
		MQ2_State_g = MQ2_SMOKE;
	}
	else if (MQ2_State_g != MQ2_SMOKE || MQ2_Millivolts_g <= MQ2_ALARM_OFF_MV)
	{
		MQ2_State_g = MQ2_CLEAR;
	}
	// Otherwise stay in MQ2_SMOKE until the level drops below MQ2_ALARM_OFF_MV

	return MQ2_State_g;
}

// Voltage at the sensor's AO pin from the last reading, in millivolts
uint32_t MQ2_GetMillivolts(void)
{
	return MQ2_Millivolts_g;
}

uint32_t MQ2_GetWarmupSecondsLeft(void)
{
	uint32_t Elapsed = HAL_GetTick() - MQ2_StartTick_g;

	if (Elapsed >= MQ2_WARMUP_MS)
	{
		return 0;
	}
	return (MQ2_WARMUP_MS - Elapsed + 999) / 1000;
}
