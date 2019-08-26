/*
 * system_watchdog.c
 *
 *  Created on: 12 lug 2019
 *      Author: JBFourierous
 */

#include "system_watchdog.h"

// ------------------------------------------------- INNER FUNCTIONS ----------------------------------------------------------------- //

/** @brief Checks voltage supply level of the MCU
 *  @param  none
 *  @return true if the device is correctly supplied, false otherwise
 */
static bool check_power_status() {
	uint16_t vref_cal = 0;
	float32_t vref_adc = 0, vdd = 0, vreff_cal = 0;
	vref_cal = *(__IO uint16_t *)((uint32_t)0x1FFF7A2A);		// read the preloaded reference voltage in memory
	vref_adc = dma_adc_buffer[4];
	vref_adc = 3.3f*(float32_t)vref_adc/4096.0f;				// convert in float with respect to max voltage
	vreff_cal = 3.0f*(float32_t)vref_cal/4096.0f;
	vdd = 3.3*(vreff_cal/vref_adc);								// current supply voltage
	if(vdd < 2)													// supply voltage too close to the 1.7V limit
		return false;
	else
		return true;
}

/** @brief Tries multiple times to check the status of the querable devices
 *  @param dev_id identifier of the device to check
 *  @return true if the device is correctly initialized, false otherwise
 */
static bool check_device(uint8_t dev_id) {
	bool tmp = false;
	uint8_t trials = BOOTING_TRIALS;
	do {
		switch(dev_id) {
			case 1:
				tmp = camera_check();
				break;
			case 2:
				tmp = GSM_check();
				break;
			case 3:
				tmp = microphone_check();
				break;
			default:
				break;
		}
		trials = trials - 1;
	} while((trials > 0) && !tmp);
	return tmp;
}

// ------------------------------------------------- END OF INNER FUNCTIONS ---------------------------------------------------------- //

// ---------------------------------------------------- INTERFACE FUNCTIONS ---------------------------------------------------------- //

/** @brief Initializes the Ranger Vervet system, device by device
 *  @param  none
 *  @return True if the system is correctly initialized, false otherwise
 */
bool system_init() {
	bool tmp = true;
	// Enable microphones
	HAL_TIM_Base_Start(&htim3);						// timer that triggers ADC conversion
	microphone_mode(true);							// turn on microphones so that ADC conversion for alla analog data is on
	HAL_Delay(1000);								// wait some time to let ADC do microphones checking
	tmp = check_power_status();						// verify board supply level
	if(!tmp)
		return false;
	else {
		camera_init();						// initialize camera
		if(GSM_init() && GSM_GPRS_init()) {							// initialize GSM board
			// verify that external devices are ok
			tmp = check_device(1) && check_device(2) && check_device(3);
			microphones_booting = false;
			if(!tmp)								// if any querable device is not ok return false
				return false;
			else {									// initialize remaining devices and return true
				// Enable LED matrix
				init_led_matrix();
				init_motor(&end_of_run);
				return tmp;
			}
		} else {
			return false;
		}
	}
}

/** @brief Checks the dynamic parameters of the system to verify that is correctly working
 *  @param  none
 *  @return true if the device is correctly working, false otherwise
 */
bool check_system_status() {
	bool tmp = true;
	tmp = GSM_check() && (long_classification_time < 5) && check_power_status();
	return tmp;
}

/** @brief Controls the alarm handling procedure
 *  @param  alarm_type indicator of the detected alarm
 *  @return none
 */
void alarm_timeout() {
	uint32_t current_time = HAL_GetTick();
	uint32_t alarm_duration = current_time - alarm_start_time;
	switch(current_alarm) {
		case 'e':
			if(alarm_duration >= EAG_TIMEOUT) {
				leds_on = false; 					// prevents leds from turning on
				stop_alarm_sound();					// prevents speaker from generating sound
				current_alarm = 'n';				// sets neutral alarm
				microphone_mode(true);				// enables audio recording
			}
			break;
		case 'l':
			if(alarm_duration >= LEO_TIMEOUT) {
				leds_on = false;					// prevents leds from turning on
				stop_alarm_sound();		    		// prevents speaker from generating sound
				motor_default(audio_source);		// brings camera back in neutral position
				camera_mode(false);					// sets camera in sleep mode
				GSM_mode(false);				    // sets GSM board in sleep mode
				current_alarm = 'n';				// sets neutral alarm
				microphone_mode(true);				// enables audio recording
			}
			break;
		case 's':
			if(alarm_duration >= SNK_TIMEOUT) {
				stop_alarm_sound();		   		    // prevents speaker from generating sound
				current_alarm = 'n';				// sets neutral alarm
				microphone_mode(true);				// enables audio recording
			}
			break;
		default:
			current_alarm = 'n';					// sets neutral alarm
			break;
	}
}

/** @brief Shuts down the Ranger Vervet system
 *  @param  none
 *  @return none
 */
void system_shutdown() {
	camera_mode(false);									// put camera in sleep mode
	GSM_mode(false);									// put GSM module in sleep mode

	// Put MCU board in Standby mode, using STM procedure. To wake up press RESET button
	/* Disable all used wakeup sources: Pin1(PA.0) */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* Clear all related wakeup flags */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	/* Re-enable all used wakeup sources: Pin1(PA.0) */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	/*## Enter Standby Mode ####################################################*/
	/* Request to enter STANDBY mode  */
	HAL_PWR_EnterSTANDBYMode();
}

// --------------------------------------------------- END OF INTERFACE FUNCTIONS ---------------------------------------------------- //

