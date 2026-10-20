/*
 * File: MQ2.h
 * Driver Name: [[ MQ2 Smoke / Gas Sensor ]]
 * SW Layer:   ECUAL
 * Created on: Oct 20, 2026
 * -------------------------------------------
 * Reads the analog output (AO) of an MQ-2 sensor module through one ADC channel.
 *
 * The module must run on 5V (its heater needs it), so AO can rise above 3.3V.
 * Feed AO to the MCU through a voltage divider:
 *     AO --[ R_TOP ]--+--> ADC pin
 *                     |
 *                 [ R_BOTTOM ]
 *                     |
 *                    GND
 */

#ifndef MQ2_H_
#define MQ2_H_

#include "main.h"

//-----[ Configuration ]-----

#define MQ2_VREF_MV             3300    // ADC reference voltage (VDDA on the Nucleo board)
#define MQ2_DIVIDER_TOP_OHMS    10000   // Resistor from sensor AO to the ADC pin (0 = no divider)
#define MQ2_DIVIDER_BOTTOM_OHMS 20000   // Resistor from the ADC pin to GND
#define MQ2_SAMPLES             16      // ADC samples averaged per reading
#define MQ2_WARMUP_MS           10000   // Heater warm-up time before readings are trusted
#define MQ2_ALARM_ON_MV_LOW         2000    // AO voltage at or above which smoke is reported
#define MQ2_ALARM_ON_MV_HIGH		3000    // AO voltage at or above which the air is very toxic (bar full)
#define MQ2_ALARM_OFF_MV        1800    // AO voltage at or below which the alarm clears (hysteresis)
#define MQ2_CLEAN_AIR_MV        0       // AO voltage in clean air = 0% on the level bar (set from a real reading)

typedef enum
{
	MQ2_WARMING_UP,
	MQ2_CLEAR,
	MQ2_TOXIC,
	MQ2_VERY_TOXIC
}MQ2_State_t;

//-----[ Prototypes For All User External Functions ]-----

void        MQ2_Init(ADC_HandleTypeDef* ADC_Handle);
MQ2_State_t MQ2_Update(void);
uint32_t    MQ2_GetMillivolts(void);
uint8_t     MQ2_GetAlarmPercent(void);
uint32_t    MQ2_GetWarmupSecondsLeft(void);

#endif /* MQ2_H_ */
