/*
 * common.h
 *
 *  Created on: 19 lug 2019
 *      Author: JBFourierous
 */

#ifndef COMMON_H_
#define COMMON_H_

// Include system core for external hardware and define global variables to interconnect the modules

#include <stdbool.h>
#include <stdint.h>
#include "arm_math.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"
#include "tim.h"

#define SIZE 5000
#define IMG_ROWS 120
#define IMG_COLS 320

/* Miscellaneous global variables */
extern bool leds_on;				     				// boolean flag that controls led signaling
extern uint32_t alarm_start_time;		 				// time of the alarm serving start
extern char current_alarm;			     				// current alarm detected
extern bool system_ok;				     				// system status indication
extern uint8_t long_classification_time; 				// counter of the number of too long classification procedures

/* Audio capture related variables */
extern float32_t pow1,pow2,pow3;				 // audio sources signal power
extern uint16_t count;							 // audio samples captured counter
extern bool microphones_booting;				 // flag to stop audio recording while checking microphones status
extern uint8_t audio_source;					 // indicates audio source position, 0 center, 1 left, 2 right
extern uint8_t microphone_status;			     // indicates that microphones are correctly working if true, otherwise false

/* ADC conversion buffer */
extern uint32_t dma_adc_buffer[5]; 			     // the first three slots are reserved for audio capture, the fourth slot saves the
												 // value of light intensity of the photoresistor to initialize the motor, the last slot
												 // saves the current supply voltage

/* Motor related variables */
extern bool motor_booting;						 // indicates whether the motor is in initialization phase or not
extern bool end_of_run;							 // indicates if the motor has reached the end of run

/* GSM module related variables */
extern uint8_t GSM_response[100];				 // GSM module responses buffer
extern bool GSM_new_response;					 // indicates if a new response from GSM module is available
extern uint8_t GSM_new_response_size;			 // indicates GSM expected response size
extern bool GSM_booting;						 // indicates if GSM board is in initialization or not

#endif /* COMMON_H_ */
