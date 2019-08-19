/*
 * speaker_alarm.c
 *
 *  Created on: 09 lug 2019
 *      Author: JBFourierous
 */

#include "speaker_alarm.h"

/** @brief  Given an assigned period, generates an alarm sound at the corresponding frequency
 *  @param  frequency Integer representing the alarm frequency
 *  @param  htim      timer handler managing the PWM generation
 *  @return none
 */
void start_alarm_sound(uint16_t frequency) {
	// For low frequency sound set frequency = 419, for high frequency set frequency = 3
	// UpdateEvent = TIM_CLK/(PSC+1)(PERIOD+1)
	// PERIOD = 419 -> UpdateEvent = 200Hz
	// PERIOD = 3 -> UpdateEvent = 20kHz
	// Set PWM period
	__HAL_TIM_SET_AUTORELOAD(&htim1, frequency);
	// Set PWM pulse half of period
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (frequency+1)/2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

/** @brief  Stops the sound generation by putting the PWM duty cycle to zero
 *  @param  htim timer handler managing the PWM generation
 *  @return none
 */
void stop_alarm_sound() {
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}
