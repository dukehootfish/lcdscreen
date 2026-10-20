/*
 * File: BUZZER.c
 * Driver Name: [[ PWM Buzzer ]]
 * SW Layer:   ECUAL
 * Created on: Oct 20, 2026
 *
 */

#include "BUZZER.h"

/*-----------------------[INTERNAL VARIABLES]-----------------------*/

static TIM_HandleTypeDef* BUZZER_TIM_Handle_g;
static uint32_t BUZZER_Channel_g;

/*-----------------------------------------------------------------------*/

//=========================================================================================================================

/*-----------------------[USER EXTERNAL FUNCTIONS]-----------------------*/

// Starts the PWM output silent (0% duty = pin held low)
void BUZZER_Init(TIM_HandleTypeDef* TIM_Handle, uint32_t Channel)
{
	BUZZER_TIM_Handle_g = TIM_Handle;
	BUZZER_Channel_g = Channel;

	__HAL_TIM_SET_COMPARE(BUZZER_TIM_Handle_g, BUZZER_Channel_g, 0);
	HAL_TIM_PWM_Start(BUZZER_TIM_Handle_g, BUZZER_Channel_g);
}

// Plays a tone at FreqHz with 50% duty (loudest). FreqHz = 0 turns it off.
void BUZZER_Tone(uint32_t FreqHz)
{
	if (FreqHz == 0)
	{
		BUZZER_Off();
		return;
	}
	if (FreqHz > BUZZER_TIMER_TICK_HZ / 2)
	{
		FreqHz = BUZZER_TIMER_TICK_HZ / 2;
	}

	uint32_t Period = BUZZER_TIMER_TICK_HZ / FreqHz;
	// Both registers are preloaded, so the new tone starts cleanly at the next period
	__HAL_TIM_SET_AUTORELOAD(BUZZER_TIM_Handle_g, Period - 1);
	__HAL_TIM_SET_COMPARE(BUZZER_TIM_Handle_g, BUZZER_Channel_g, Period / 2);
}

void BUZZER_Off(void)
{
	__HAL_TIM_SET_COMPARE(BUZZER_TIM_Handle_g, BUZZER_Channel_g, 0);
}
