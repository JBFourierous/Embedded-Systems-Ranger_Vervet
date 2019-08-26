/*
 * led_matrix.h
 *
 *  Created on: 11 lug 2019
 *      Author: JBFourierous
 */

#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

#include "common.h"
#include "stm32f4xx_hal_tim.h"

#define N_LEDS 32
#define INTENSITY 50
#define LED_BYTE 24
#define H_VAL 75
#define L_VAL 30

static const uint8_t WHITE[] = {50, 50, 50};
static const uint8_t BLACK[] = {0, 0, 0};

void turn_on_led_matrix();
void turn_off_led_matrix();
void init_led_matrix();

#endif /* LED_MATRIX_H_ */

