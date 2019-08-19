/*
 * camera.h
 *
 *  Created on: 03 ago 2019
 *      Author: JBFourierous
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "OV7670_register_definitions.h"
#include "common.h"
#include "i2c.h"
#include "usart.h"

#define VSYNC_PORT GPIOB
#define VSYNC_PIN GPIO_PIN_4
#define PCLK_PORT GPIOB
#define PCLK_PIN GPIO_PIN_5
#define OV7670_READ_ADDR 0x43
#define OV7670_WRITE_ADDR 0x42
#define ROWS 120
#define COLS 160

void camera_init();
bool camera_check();
void camera_mode(bool mode);
void camera_capture_image();
void camera_capture_image_PC();

#endif /* CAMERA_H_ */
