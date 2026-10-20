/*
 * File: BUZZER.h
 * Driver Name: [[ PWM Buzzer ]]
 * SW Layer:   ECUAL
 * Created on: Oct 20, 2026
 * -------------------------------------------
 * Plays a square-wave tone on a buzzer from one timer PWM channel.
 * Made for a passive buzzer (it needs a tone to make sound). An active
 * buzzer also beeps with it, but sounds its own built-in pitch.
 *
 * The timer must already be set up in PWM mode by MX_TIMx_Init().
 * BUZZER_TIMER_TICK_HZ must match its clock: timer clock / (Prescaler + 1).
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "main.h"

//-----[ Configuration ]-----

#define BUZZER_TIMER_TICK_HZ    1000000 // TIM2: 170 MHz / (169 + 1) = 1 MHz

//-----[ Prototypes For All User External Functions ]-----

void BUZZER_Init(TIM_HandleTypeDef* TIM_Handle, uint32_t Channel);
void BUZZER_Tone(uint32_t FreqHz);
void BUZZER_Off(void);

#endif /* BUZZER_H_ */
