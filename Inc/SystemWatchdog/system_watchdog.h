/*
 * system_watchdog.h
 *
 *  Created on: 09 ago 2019
 *      Author: JBFourierous
 */

#ifndef SYSTEM_WATCHDOG_H_
#define SYSTEM_WATCHDOG_H_

#include "common.h"
#include "camera.h"
#include "microphone.h"
#include "led_matrix.h"
#include "speaker_alarm.h"
#include "motor_driver.h"
#include "GSM_board.h"

#define BOOTING_TRIALS 3
#define EAG_TIMEOUT 5000
#define LEO_TIMEOUT 10000
#define SNK_TIMEOUT 5000

bool system_init();
bool check_system_status();
void alarm_timeout();
void system_shutdown();

#endif /* SYSTEM_WATCHDOG_H_ */
