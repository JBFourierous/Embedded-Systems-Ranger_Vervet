/*
 * speaker_alarm.h
 *
 *  Created on: 09 lug 2019
 *      Author: JBFourierous
 */

#include "common.h"
#include "stm32f4xx_hal_tim.h"

#ifndef SPEAKER_ALARM_H_
#define SPEAKER_ALARM_H_

#define ULTRASOUND_FREQ 25000
#define INFRASOUND_FREQ 100

void start_alarm_sound(uint16_t frequency, TIM_HandleTypeDef* htim);
void stop_alarm_sound(TIM_HandleTypeDef* htim);

#endif /* SPEAKER_ALARM_H_ */
