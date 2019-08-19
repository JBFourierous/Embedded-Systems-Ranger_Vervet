/*
 * alarm_handler.c
 *
 *  Created on: 06 lug 2019
 *      Author: JBFourierous
 */

#include "alarm_handler.h"
#include "GSM_board.h"

uint32_t alarm_start_time = 0;

/** @brief  According to the alarm signaled handles it with the corresponding procedure
*   @param  alarm_type char indicating the alarm type
*   @return none
*/
void handle_alarm(char alarm_type) {
	switch(alarm_type) {
		// eagle alarm signaled, handling procedure:
		// - stop audio recording
		// - set the general alarm flag to 'e' to acknowledge system watchdog
		// - turn on leds to scare the bird;
		// - generate high frequency sound to scare the bird;
		case 'e':
			microphone_mode(false);
			current_alarm = 'e';
			alarm_start_time = HAL_GetTick();
			leds_on = true;
			start_alarm_sound(ULTRASOUND_FREQ);
			break;
		// land alarm signaled, handling procedure:
	    // - stop audio recording
		// - set the general alarm flag to 'l' to acknowledge system watchdog
		// - turn on leds to scare the attacking animal or to warn the ill-intentioned;
		// - generate high frequency sound to scare the possible attacking animal;
		// - wake up GSM board;
		// - wake up camera board;
		// - turn camera in sound direction and take a picture;
		// - send the picture via GPRS network;
		case 'l':
			microphone_mode(false);
			current_alarm = 'l';
			alarm_start_time = HAL_GetTick();
			//if(!GSM_mode(true))							// unable to wakeup GSM
				//break;
			camera_mode(true);
			motor_turn(audio_source);
			camera_capture_image_PC();
			start_alarm_sound(ULTRASOUND_FREQ);
			leds_on = true;
			//if(!GSM_send_HTTP())						// if unable to send image via HTTP send a warning message
				//GSM_send_SMS();
			//GSM_send_SMS();
			break;
		// snake alarm signaled, handling procedure:
		// - stop audio recording
		// - set the general alarm flag to 's' to acknowledge system watchdog
		// - generate low frequency sound to scare the snake
		case 's':
			microphone_mode(false);
			current_alarm = 's';
			alarm_start_time = HAL_GetTick();
			start_alarm_sound(INFRASOUND_FREQ);
			break;
		// default case handling
		default:
			current_alarm = 'n';
			break;
	}
}

