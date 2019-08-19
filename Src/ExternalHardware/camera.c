/*
 * camera.c
 *
 *  Created on: 03 ago 2019
 *      Author: JBFourierous
 */

#include "camera.h"

// These values set the color mode in RGB565, 16-bits depth
const RegisterData regsRGB565 [] = {
    {REG_COM7, COM7_RGB}, /* Selects RGB mode */
    {REG_RGB444, 0},	  /* No RGB444 please */
    {REG_COM1, 0x01},
    {REG_COM15, COM15_RGB565|COM15_R00FF},
    {REG_COM9, 0x4A},	 /* 32x gain ceiling; 0x8 is reserved bit */
    {REG_COM13, COM13_UVSAT},
    {0xff, 0xff}	/* END MARKER */
};

// These values set the resolution to 160x120
const RegisterData regsQQVGA [] = {
    {REG_COM3, COM3_DCWEN}, // enable downsamp/crop/window
    {REG_COM14, 0x1a},	// divide by 4
    {0x72, 0x22},		// downsample by 4
    {0x73, 0xf2},		// divide by 4
    {REG_HSTART,0x16},
    {REG_HSTOP,0x04},
    {REG_HREF,0xa4},
    {REG_VSTART,0x02},
    {REG_VSTOP,0x7a},
    {REG_VREF,0x0a},
    {0xff, 0xff},	/* END MARKER */
};

// These values set the camera features
const RegisterData regsDefault [] = {
	// gamma values
	{0x7a, 0x40},		{0x7b, 0x04},
	{0x7c, 0x08},		{0x7d, 0x10},
	{0x7e, 0x20},		{0x7f, 0x28},
	{0x80, 0x30},		{0x81, 0x38},
	{0x82, 0x40},		{0x83, 0x48},
	{0x84, 0x50},		{0x85, 0x60},
	{0x86, 0x70},		{0x87, 0x90},
	{0x88, 0xb0},		{0x89, 0xd0},

	// AWB, AEC, AGC
	{REG_COM8, 0xe5}, {AWBCTR0, 0x9a}, {REG_HAECC7, 0x04},
	{REG_BLUE, 0x6f}, {REG_RED, 0x8f},
	{REG_AEW, 0x75}, {REG_AEB, 0x63}, {REG_VPT, 0xd4},
	// Saturation
	{MTX1, 0x99}, {MTX2, 0x99},
	{MTX3, 0x00}, {MTX4, 0x28},
	{MTX5, 0x71}, {MTX6, 0x99},
	{MTXS, 0x9e},
	// Brightness
	{REG_BRIGHT, 0x90},
	// Contrast
	{REG_CONTRAS, 0x55},
	// ABLC
	{0xb1, 0x0c}, {0xb3, 0x8f},
	{0xb5, 0x44}, {0xbe, 0x5f},
	{0xbf, 0x4f}, {COM17_AECWIN, 0x0f},
	{0xc1, 0x0f},

	{0xff, 0xff},	/* END MARKER */
};


uint8_t line_buffer[COLS*2+1+5];
uint8_t line_count = ROWS, line_length = COLS;
static const uint32_t send_read  = 4;
uint16_t line_buffer_index = 0;

// ------------------------------------------------------------------------------------------------------------------------------- //
// Functions to send an image directly to a PC. The image is shown by a small Java program reading serial port, waiting for display
// commands and pixel bytes

// Sends new image frame command
static void start_new_frame(uint8_t pixelFormat) {
  uint8_t data = 0x00;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
  data = 0x01;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);

  // frame width
  data = (line_length >> 8) & 0xFF;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
  data = line_length & 0xFF;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);

  // frame height
  data = (line_count >> 8) & 0xFF;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
  data = line_count & 0xFF;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);

  // pixel format
  data = pixelFormat;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}

// Sends upper byte of pixel
static void send_pixel_byte_high(uint8_t byte) {
  //              RRRRRGGG
  uint8_t data = byte | 0b00001000; // make pixel color always slightly above 0 since zero is end of line marker
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}

// Sends lower byte of pixel
static void send_pixel_byte_low(uint8_t byte) {
  //              GGGBBBBB
  uint8_t data = byte | 0b00001001; // make pixel color always slightly above 0 since zero is end of line marker
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}

static void send_pixel_byte_PC() {
	uint8_t byte = line_buffer[line_buffer_index];
	uint8_t low_byte = line_buffer_index & 0x01;

	if (low_byte) {
		send_pixel_byte_low(byte);
	} else {
		send_pixel_byte_high(byte);
	}
	line_buffer_index++;
}

// Sends the image end of line command
static void end_of_line()   {
  uint8_t data = 0x00;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
  data = 0x02;
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}
// ------------------------------------------------------------------------------------------------------------------------------------ //

// ------------------------------------------------------- INNER FUNCTIONS ------------------------------------------------------------ //

/** @brief Sets a register value
 *  @param  addr address of the register to set
 *  @param  data value to write
 *  @return none
 */
static bool ov7670_write_reg(uint8_t addr, uint8_t data) {
  uint8_t ret;
  do {
    ret = HAL_I2C_Mem_Write(&hi2c1, OV7670_WRITE_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
  } while (ret != HAL_OK && 0);
  return true;
}

/** @brief Reads a register value
 *  @param  addr address of the register to read
 *  @return value of the read register
 */
static uint8_t ov7670_read_reg(uint8_t addr) {
  uint8_t ret, data;
  do {
    ret = HAL_I2C_Master_Transmit(&hi2c1, OV7670_WRITE_ADDR, &addr, 1, 100);
    ret |= HAL_I2C_Master_Receive(&hi2c1, OV7670_READ_ADDR, &data, 1, 100);
  } while (ret != HAL_OK && 0);
  return data;
}

/** @brief Writes a group of registers
 *  @param  data pointer to the group of registers to write
 *  @return none
 */
static void ov7670_multi_write_reg(const RegisterData* data) {
	volatile int i = 0;
	RegisterData reg_data = data[i];
	uint8_t reg_addr = reg_data.addr;
	uint8_t reg_val = reg_data.val;
	while(reg_addr != 0xff) {
		ov7670_write_reg(reg_addr, reg_val);
		  HAL_Delay(10);
		i++;
		reg_data = data[i];
		reg_addr = reg_data.addr;
		reg_val = reg_data.val;
	}
}

/** @brief Checks if the camera registers are correctly set
 *  @param  reg_data pointer to a set of registers to check
 *  @return true if camera is correctly initialized, false otherwise
 */
static int regs_check(const RegisterData* reg_data) {
	volatile uint8_t reg_addr = 0, reg_val = 0, tmp = 0, i = 0;
	bool res = true;
	int errors = 0;
	RegisterData curr = reg_data[i];
	reg_addr = curr.addr;
	reg_val = curr.val;
	tmp = 0;
	while(reg_addr != 0xff) {
		tmp = ov7670_read_reg(reg_addr);
		res = res && (tmp == reg_val);
		if(!res)
			errors++;
		HAL_Delay(10);
		i++;
		curr = reg_data[i];
		reg_addr = curr.addr;
		reg_val = curr.val;
	}
	return errors;
}

// ----------------------------------------------- END INNER FUNCTIONS ---------------------------------------------------------------- //

// ----------------------------------------------------- INTERFACE FUNCTIONS ---------------------------------------------------------- //

/** @brief Initializes the camera setting its registers
 *  @param  hi2c pointer to I2C protocol handler
 *  @return none
 */
void camera_init() {
	// reset camera
	ov7670_write_reg(0x12, 0x80);
	HAL_Delay(50);
	// set color mde, resolution and other settigs
	ov7670_multi_write_reg(regsRGB565);
	ov7670_multi_write_reg(regsQQVGA);
	ov7670_multi_write_reg(regsDefault);
	// Defect pixel adjustment
	ov7670_write_reg(0x76, ov7670_read_reg(0x76)|0xff);
	// adjust timing signals
	ov7670_write_reg(REG_COM10, ov7670_read_reg(REG_COM10)|COM10_PCLK_HB);		// PCLK does not toggle during blanking
	ov7670_write_reg(REG_COM6, ov7670_read_reg(REG_COM6)|COM6_HREF_HB);
	ov7670_write_reg(REG_CLKRC, 40); 											// internal clock prescaling
}

/** @brief Checks if camera registers are correctly set
 *  @param  none
 *  @return true if the camera is correctly initialized, false otherwise
 */
bool camera_check() {
	return regs_check(regsRGB565) && regs_check(regsQQVGA) && regs_check(regsDefault);
}


/** @brief Sets the camera operating mode
 *  @param  mode if true the camera is turn on, if false the camera is in standby mode
 *  @return none
 */
void camera_mode(bool mode) {
	if(!mode)
		ov7670_write_reg(REG_COM2, COM2_SSLEEP);
	else
		ov7670_write_reg(REG_COM2, 0x00);
}

/** @brief Captures an image and sends it via UART
 *  @param  none
 *  @return none
 */
void camera_capture_image() {
	volatile int n = 0;
	// Read atomically VSYNC pin
	while(!((uint8_t)GPIOB->IDR & 0b00010000));
	for (uint16_t y = 0; y < ROWS; y++) {
		line_buffer_index = 0;
		uint8_t send_count = 0;
		line_buffer[0] = 0; 									// first byte from Camera is half a pixel
		for (uint16_t x = 1; x < COLS*2+1; x++) {
			// start sending first bytes while reading pixels from camera
			if (send_read > 0) {
				if (send_count) {
					send_count--;
				} else {
					HAL_UART_Transmit_DMA(&huart1, &line_buffer[line_buffer_index], 1);
					send_count = send_read;
					line_buffer_index++;
				}
			}
			// Read atomically PCLK
			while((uint8_t)GPIOB->IDR & 0b00100000);
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			line_buffer[x] = ((uint16_t)GPIOC->IDR) >> 8;
		}
		// send rest of the line
		while (line_buffer_index < line_length * 2) {
			HAL_UART_Transmit_DMA(&huart1, &line_buffer[line_buffer_index], 1);
			line_buffer_index++;
		}
	}
}

// Only for demonstration purpose
/** @brief Captures an image and sends it via UART to a PC
 *  @param  none
 *  @return none
 */
void camera_capture_image_PC() {
	start_new_frame(1);
	// Read atomically VSYNC pin
	while(!((uint8_t)GPIOB->IDR & 0b00010000));
	for (uint16_t y = 0; y < ROWS; y++) {
		line_buffer_index = 0;
		uint8_t send_count = 0;
		line_buffer[0] = 0; 									// first byte from Camera is half a pixel
		for (uint16_t x = 1; x < COLS*2+1; x++) {
			// start sending first bytes while reading pixels from camera
			if (send_read > 0) {
				if (send_count) {
					send_count--;
				} else {
					send_pixel_byte_PC();
					send_count = send_read;
				}
			}
			// Read atomically PCLK 
			while((uint8_t)GPIOB->IDR & 0b00100000);
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			line_buffer[x] = ((uint16_t)GPIOC->IDR) >> 8;
		}
		// send rest of the line
		while (line_buffer_index < line_length * 2) {
			send_pixel_byte_PC();
		}
		end_of_line();
	}
}

// --------------------------------------------------- END INTERFACE FUNCTIONS ------------------------------------------------------- //



