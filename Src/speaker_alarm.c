/*
 * speaker_alarm.c
 *
 *  Created on: 09 lug 2019
 *      Author: JBFourierous
 */

#include "speaker_alarm.h"

/** @brief  Given an assigned frequency, generates an alarm sound at that frequency setting the ARR timer register
 *  @param  frequency Integer representing the alarm frequency
 *  @param  htim      timer handler managing the PWM generation
 *  @return none
 */
void start_alarm_sound(uint16_t frequency, TIM_HandleTypeDef* htim) {
	__HAL_TIM_SET_AUTORELOAD(htim2, frequency);
	__HAL_TIM_SET_COMPARE(htim2, TIM_CHANNEL_2, 800);
}

/** @brief  Stops the sound generation by putting the PWM duty cycle to zero
 *  @param  htim timer handler managing the PWM generation
 *  @return none
 */
void stop_alarm_sound(TIM_HandleTypeDef* htim) {
	__HAL_TIM_SET_COMPARE(htim2, TIM_CHANNEL_2, 0);
}
