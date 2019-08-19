/*
 * motor_driver.h
 *
 *  Created on: 09 lug 2019
 *      Author: JBFourierous
 */
#include "common.h"

#ifndef INC_MOTOR_DRIVER_H_
#define INC_MOTOR_DRIVER_H_

#define CENTRE 0
#define RIGHT 1
#define LEFT 2
#define SEQUENCES 64

void motor_turn(uint8_t direction);
void motor_default(uint8_t current_position);
void init_motor(bool* end_of_run);

#endif /* INC_MOTOR_DRIVER_H_ */
