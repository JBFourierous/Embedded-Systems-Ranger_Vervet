/*
 * microphone.h
 *
 *  Created on: 18 lug 2019
 *      Author: JBFourierous
 */

#ifndef MICROPHONE_H_
#define MICROPHONE_H_

#include "common.h"
#include "classifier.h"
#include "alarm_handler.h"
#include "usart.h"

bool microphone_check();
void record_audio();
void microphone_mode(bool mode);

#endif /* MICROPHONE_H_ */
