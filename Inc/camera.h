/*
 * camera.h
 *
 *  Created on: 16 lug 2019
 *      Author: JBFourierous
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "OV7670_register_definitions.h"
#include <stdbool.h>
#include "i2c.h"

#define VSYNC_PORT GPIOB
#define VSYNC_PIN GPIO_PIN_4
#define PCLK_PORT GPIOB
#define PCLK_PIN GPIO_PIN_5
#define HREF_PORT GPIOB
#define HREF_PIN GPIO_PIN_3
#define OV7670_READ_ADDR 0x43
#define OV7670_WRITE_ADDR 0x42
#define ROWS 120
#define COLS 160

void camera_init(I2C_HandleTypeDef* hi2c);
int camera_check();
void camera_mode(bool mode);
int camera_capture_image(uint8_t image_buffer[][2*COLS]);
int camera_capture_image2(uint8_t image_buffer[][2*COLS]);

#endif /* CAMERA_H_ */
