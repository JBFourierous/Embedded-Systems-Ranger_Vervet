/*
 * led_matrix.c
 *
 *  Created on: 11 lug 2019
 *      Author: JBFourierous
 */

#include "led_matrix.h"

uint32_t led_matrix_message[(N_LEDS+1)*LED_BYTE];      // message encoding coloration of leds

// --------------------------------------------------- INNER FUNCTIONS ----------------------------------------------------------- //

/** @brief Given a LED, sets its color
 *  @param  ledIdx LED identifier
 *  @param  color color to be set
 *  @return none
 */
static void ws2812_setColor(int led_id, const uint8_t *color) {
	if (led_id >= N_LEDS)
		return;

	uint8_t r = color[0];
	uint8_t g = color[1];
	uint8_t b = color[2];

	int i = led_id*LED_BYTE;
	uint8_t mask;
	mask = 0x80;
	while (mask) {
		led_matrix_message[i] = (mask & g) ? H_VAL : L_VAL;
		mask >>= 1;
		i++;
	}
	mask = 0x80;
	while (mask) {
		led_matrix_message[i] = (mask & r) ? H_VAL : L_VAL;
		mask >>= 1;
		i++;
	}
	mask = 0x80;
	while (mask) {
		led_matrix_message[i] = (mask & b) ? H_VAL : L_VAL;
		mask >>= 1;
		i++;
	}
}

/** @brief Applies the set color to all the matrix LEDs
 *  @param  none
 *  @return none
 */
static void ws2812_applyColors() {
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t*)led_matrix_message,
			sizeof(led_matrix_message) / sizeof(led_matrix_message[0]));
}

/** @brief Turns off the LED matrix
 *  @param  none
 *  @return none
 */
static void ws2812_turnOffLeds() {
	// Set each LED to black colour
	for (int i = 0; i < N_LEDS; i++)
		ws2812_setColor(i, BLACK);
	ws2812_applyColors();
}

// ----------------------------------------------------- END INNER FUNCTIONS --------------------------------------------------------- //


// ----------------------------------------------------- INTERFACE FUNCTIONS --------------------------------------------------------- //

/** @brief Initializes the LED matrix
 *  @param  none
 *  @return none
 */
void init_led_matrix() {
	for (int i = 0; i < (N_LEDS+1)*LED_BYTE; i++)
		led_matrix_message[i] = 0;
	turn_off_led_matrix();
}

/** @brief Turns off the LED matrix
 *  @param  none
 *  @return none
 */
void turn_off_led_matrix() {
	ws2812_turnOffLeds();
}


/** @brief Lights up the LED matrix
 *  @param  none
 *  @return none
 */
void turn_on_led_matrix() {
	uint32_t delay = (uint32_t)(ceilf(255.0/100)*2.0f);
	int i;
	for(i = 0; i < N_LEDS; i++) {
		ws2812_setColor(i, WHITE);
	}
	ws2812_applyColors();
}

// ----------------------------------------------------- END INTERFACE FUNCTIONS ------------------------------------------------------ //

