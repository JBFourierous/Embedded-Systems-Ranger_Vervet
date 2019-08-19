/*
 * classifier.h
 *
 *  Created on: 27 lug 2019
 *      Author: JBFourierous
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include "common.h"
#include "arm_math.h"

#define LMS_SIZE 10
#define HIGHPASS_SIZE 200
#define INPUT_SIZE 5000
#define BLOCK_SIZE 50

char classify(float32_t* input);

#endif /* CLASSIFIER_H_ */
