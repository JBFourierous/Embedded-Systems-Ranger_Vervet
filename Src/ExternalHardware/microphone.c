/*
 * microphone.c
 *
 *  Created on: 18 lug 2019
 *      Author: JBFourierous
 */
#include "microphone.h"

ADC_HandleTypeDef hadc1;
static float32_t audio_sampled1[5000];		     // Input audio buffer
static float32_t audio_sampled2[5000];			 // Input audio buffer
static bool switch_audio_buffer = false;		 // Flag to change audio buffer while input signal is analyzed

float32_t pow1 = 0, pow2 = 0, pow3 = 0;			 // audio signals power
uint8_t audio_source = 0;						 // audio signal direction

// -------------------------------------------------- INTERFACE FUNCTIONS ----------------------------------------------------------- //

/**
 * @brief  Checks if microphones work properly
 * @param  none
 * @return if true microphones work correctly otherwise returns false
 */
bool microphone_check() {
	// If audio sampled is equal to ADC thesholds something is not correct
	if((dma_adc_buffer[0] == 0 || dma_adc_buffer[0] == 4095) && (dma_adc_buffer[1] == 0 || dma_adc_buffer[1] == 4095)
					&& (dma_adc_buffer[2] == 0 || dma_adc_buffer[2] == 4095))
		return false;
	else
		return true;
}

/**
 * @brief  Records the audio ADC converted sensed by microphones
 * @param  none
 * @return none
 */
void record_audio() {
  char alarm_type;

  // Convert sampled values in float
  float32_t val1 = 0, val2 = 0, val3 = 0;
  val1 = dma_adc_buffer[0]/4095.0;
  val2 = dma_adc_buffer[1]/4095.0;
  val3 = dma_adc_buffer[2]/4095.0;

  // Update signal energies
  pow1 = pow1 + val1*val1;
  pow2 = pow2 + val2*val2;
  pow3 = pow3 + val3*val3;

  // check which audio sample to store, based on maximum partial energy of the signal
  if(!switch_audio_buffer) {
	if(pow1 >pow2 && pow1 > pow3)
		audio_sampled1[count] = val1;
    else if(pow2 > pow1 && pow2 > pow3)
		audio_sampled1[count] = val2;
    else if(pow3 > pow1 && pow3 > pow2)
		audio_sampled1[count] = val3;
    else
    	audio_sampled1[count] = val1;
  } else {
	if(pow1 >pow2 && pow1 > pow3)
		audio_sampled2[count] = val1;
    else if(pow2 > pow1 && pow2 > pow3)
		audio_sampled2[count] = val2;
    else if(pow3 > pow1 && pow3 > pow2)
		audio_sampled2[count] = val3;
    else
		audio_sampled2[count] = val1;
  }
 
  // Update samples counter
  count = count + 1;

  // End of audio record, classification algorithm call
  if(count == 5000) {
	count = 0;
	if(!switch_audio_buffer)
		alarm_type = classify(audio_sampled1);
	else
		alarm_type = classify(audio_sampled2);
	// Localize audio source according to signal power
	if(pow1 > pow2) {
		if(pow1 > pow3)
			audio_source = 1;
		else
			audio_source = 2;
	} else if(pow2 > pow3)
		audio_source = 0;
	else
		audio_source = 2;
	// Handle detected alarm
	handle_alarm(alarm_type);
	switch_audio_buffer = !switch_audio_buffer;
	pow1 = 0;
	pow2 = 0;
	pow3 = 0;
  }
}

/**
 * @brief  Enables or disables audio recording
 * @param  mode if true turns on microphones else turns off microphones
 * @return none
 */
void microphone_mode(bool mode) {
	if(mode)
		HAL_ADC_Start_DMA(&hadc1, dma_adc_buffer, sizeof(dma_adc_buffer));
	else
		HAL_ADC_Stop_DMA(&hadc1);
}

// ------------------------------------------------- END INTERFACE FUNCTIONS -------------------------------------------------------- //
