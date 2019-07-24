/*
 * camera.c
 *
 *  Created on: 16 lug 2019
 *      Author: JBFourierous
 */

#include "camera.h"

// Registers settings for image format and resolution
const RegisterData regsRGB565 [] = {
    {REG_COM7, COM7_RGB}, /* Selects RGB mode */
    {REG_RGB444, 0},	  /* No RGB444 please */
    {REG_COM1, 0x01},
    {REG_COM15, COM15_RGB565|COM15_R00FF},
    {REG_COM9, 0x6A},	 /* 128x gain ceiling; 0x8 is reserved bit */
    {0x4f, 0xb3},		 /* "matrix coefficient 1" */
    {0x50, 0xb3},		 /* "matrix coefficient 2" */
    {0x51, 0},		     /* vb */
    {0x52, 0x3d},		 /* "matrix coefficient 4" */
    {0x53, 0xa7},		 /* "matrix coefficient 5" */
    {0x54, 0xe4},		 /* "matrix coefficient 6" */
    {REG_COM13, /*COM13_GAMMA|*/COM13_UVSAT},
    {0xff, 0xff}	/* END MARKER */
};

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

const RegisterData regsYUV422 [] = {
    {REG_COM7, 0x0},	/* Selects YUV mode */
    {REG_RGB444, 0},	/* No RGB444 please */
    {REG_COM1, 1},
    {REG_COM15, COM15_R00FF},
    {REG_COM9, 0x6A},	/* 128x gain ceiling; 0x8 is reserved bit */
    {0x4f, 0x80},		/* "matrix coefficient 1" */
    {0x50, 0x80},		/* "matrix coefficient 2" */
    {0x51, 0},		/* vb */
    {0x52, 0x22},		/* "matrix coefficient 4" */
    {0x53, 0x5e},		/* "matrix coefficient 5" */
    {0x54, 0x80},		/* "matrix coefficient 6" */
    {REG_COM13,/*COM13_GAMMA|*/COM13_UVSAT},
    {0xff, 0xff},
};

const RegisterData regsBase[] = {
	{REG_COM10, 0x24},
	{REG_COM6, 0xc3},
	{DBLV, 0x00},
	{REG_CLKRC, 0x1f},		// questo valore varia per il pixel clock
	{0xff, 0xff}
};

const RegisterData regsDefault [] = { //from the linux driver
                         {REG_COM7, COM7_RESET},
                         {REG_TSLB,  0x04},	/* OV */
                         {REG_COM7, 0},	    /* VGA */
    /*
     * Set the hardware window.  These values from OV don't entirely
     * make sense - hstop is less than hstart.  But they work...
     */
                         {REG_HSTART, 0x13},	{REG_HSTOP, 0x01},
                         {REG_HREF, 0xb6},	{REG_VSTART, 0x02},
                         {REG_VSTOP, 0x7a},	{REG_VREF, 0x0a},

                         {REG_COM3, 0},	{REG_COM14, 0},
    /* Mystery scaling numbers */
                         {0x70, 0x3a},		{0x71, 0x35},
                         {0x72, 0x11},		{0x73, 0xf0},
                         {0xa2,/* 0x02 changed to 1*/1},
                         {REG_COM10, 0},
    /* Gamma curve values */
                         {0x7a, 0x20},		{0x7b, 0x10},
                         {0x7c, 0x1e},		{0x7d, 0x35},
                         {0x7e, 0x5a},		{0x7f, 0x69},
                         {0x80, 0x76},		{0x81, 0x80},
                         {0x82, 0x88},		{0x83, 0x8f},
                         {0x84, 0x96},		{0x85, 0xa3},
                         {0x86, 0xaf},		{0x87, 0xc4},
                         {0x88, 0xd7},		{0x89, 0xe8},
    /* AGC and AEC parameters.  Note we start by disabling those features,
       then turn them only after tweaking the values. */
                         {REG_COM8, COM8_FASTAEC | COM8_AECSTEP},
                         {REG_GAIN, 0},	{REG_AECH, 0},
                         {REG_COM4, 0x40}, /* magic reserved bit */
                         {REG_COM9, 0x18}, /* 4x gain + magic rsvd bit */
                         {REG_BD50MAX, 0x05},	{REG_BD60MAX, 0x07},
                         {REG_AEW, 0x95},	{REG_AEB, 0x33},
                         {REG_VPT, 0xe3},	{REG_HAECC1, 0x78},
                         {REG_HAECC2, 0x68},	{0xa1, 0x03}, /* magic */
                         {REG_HAECC3, 0xd8},	{REG_HAECC4, 0xd8},
                         {REG_HAECC5, 0xf0},	{REG_HAECC6, 0x90},
                         {REG_HAECC7, 0x94},
                         {REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_AGC|COM8_AEC},
                         {0x30,0},{0x31,0},//disable some delays
    /* Almost all of these are magic "reserved" values.  */
                         {REG_COM5, 0x61},	{REG_COM6, 0x4b},
                         {0x16, 0x02},		{REG_MVFP, 0x07},
                         {0x21, 0x02},		{0x22, 0x91},
                         {0x29, 0x07},		{0x33, 0x0b},
                         {0x35, 0x0b},		{0x37, 0x1d},
                         {0x38, 0x71},		{0x39, 0x2a},
                         {REG_COM12, 0x78},	{0x4d, 0x40},
                         {0x4e, 0x20},		{REG_GFIX, 0},
    /*{0x6b, 0x4a},*/		{0x74,0x10},
                         {0x8d, 0x4f},		{0x8e, 0},
                         {0x8f, 0},		{0x90, 0},
                         {0x91, 0},		{0x96, 0},
                         {0x9a, 0},		{0xb0, 0x84},
                         {0xb1, 0x0c},		{0xb2, 0x0e},
                         {0xb3, 0x82},		{0xb8, 0x0a},

    /* More reserved magic, some of which tweaks white balance */
                         {0x43, 0x0a},		{0x44, 0xf0},
                         {0x45, 0x34},		{0x46, 0x58},
                         {0x47, 0x28},		{0x48, 0x3a},
                         {0x59, 0x88},		{0x5a, 0x88},
                         {0x5b, 0x44},		{0x5c, 0x67},
                         {0x5d, 0x49},		{0x5e, 0x0e},
                         {0x6c, 0x0a},		{0x6d, 0x55},
                         {0x6e, 0x11},		{0x6f, 0x9e}, /* it was 0x9F "9e for advance AWB" */
                         {0x6a, 0x40},		{REG_BLUE, 0x40},
                         {REG_RED, 0x60},
                         {REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_AGC|COM8_AEC|COM8_AWB},

    /* Matrix coefficients */
                         {0x4f, 0x80},		{0x50, 0x80},
                         {0x51, 0},		{0x52, 0x22},
                         {0x53, 0x5e},		{0x54, 0x80},
                         {0x58, 0x9e},

                         {REG_COM16, COM16_AWBGAIN},	{REG_EDGE, 0},
                         {0x75, 0x05},		{REG_REG76, 0xe1},
                         {0x4c, 0},		{0x77, 0x01},
                         {REG_COM13, /*0xc3*/0x48},	{0x4b, 0x09},
                         {0xc9, 0x60},		/*{REG_COM16, 0x38},*/
                         {0x56, 0x40},

                         {0x34, 0x11},		{REG_COM11, COM11_EXP|COM11_HZAUTO},
                         {0xa4, 0x82/*Was 0x88*/},		{0x96, 0},
                         {0x97, 0x30},		{0x98, 0x20},
                         {0x99, 0x30},		{0x9a, 0x84},
                         {0x9b, 0x29},		{0x9c, 0x03},
                         {0x9d, 0x4c},		{0x9e, 0x3f},
                         {0x78, 0x04},

    /* Extra-weird stuff.  Some sort of multiplexor register */
                         {0x79, 0x01},		{0xc8, 0xf0},
                         {0x79, 0x0f},		{0xc8, 0x00},
                         {0x79, 0x10},		{0xc8, 0x7e},
                         {0x79, 0x0a},		{0xc8, 0x80},
                         {0x79, 0x0b},		{0xc8, 0x01},
                         {0x79, 0x0c},		{0xc8, 0x0f},
                         {0x79, 0x0d},		{0xc8, 0x20},
                         {0x79, 0x09},		{0xc8, 0x80},
                         {0x79, 0x02},		{0xc8, 0xc0},
                         {0x79, 0x03},		{0xc8, 0x40},
                         {0x79, 0x05},		{0xc8, 0x30},
                         {0x79, 0x26},

                         {0xff, 0xff},	/* END MARKER */
};



static I2C_HandleTypeDef  *sp_hi2c;

// Inner functions for hardware interfacing

/** @brief Sets a register value
 *  @param  addr address of the register to set
 *  @param  data value to write
 *  @return none
 */
void ov7670_write_reg(uint8_t addr, uint8_t data) {
  uint8_t ret;
  do {
    ret = HAL_I2C_Mem_Write(sp_hi2c, OV7670_WRITE_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
  } while (ret != HAL_OK && 0);
}

/** @brief Reads a register value
 *  @param  addr address of the register to read
 *  @return value of the read register
 */
uint8_t ov7670_read_reg(uint8_t addr) {
  uint8_t ret, data;
  do {
    ret = HAL_I2C_Master_Transmit(sp_hi2c, OV7670_WRITE_ADDR, &addr, 1, 100);
    ret |= HAL_I2C_Master_Receive(sp_hi2c, OV7670_READ_ADDR, &data, 1, 100);
  } while (ret != HAL_OK && 0);
  return data;
}

/** @brief Writes a group of registers
 *  @param  data pointer to the group of registers to write
 *  @return none
 */
void ov7670_multi_write_reg(const RegisterData* data) {
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

/** @brief Initializes the camera setting its registers
 *  @param  none
 *  @return none
 */
static void ov7670_init(I2C_HandleTypeDef* hi2c) {
	sp_hi2c = hi2c;
	ov7670_write_reg(0x12, 0x80);
	HAL_Delay(50);
	ov7670_multi_write_reg(regsDefault);
	ov7670_multi_write_reg(regsRGB565);
	ov7670_multi_write_reg(regsQQVGA);
	ov7670_multi_write_reg(regsBase);
}

// External functions

/** @brief Public interface to initialize the camera
 *  @param  hi2c I2C handler used in the communication
 *  @return none
 */
void camera_init(I2C_HandleTypeDef* hi2c) {
	ov7670_init(hi2c);
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

/** @brief Captures an image and stores it in memory
 *  @param  none
 *  @return none
 */
int camera_capture_image(uint8_t image_buffer[][2*COLS]) {
	uint16_t hg = 120, wg = 320, lg2;
	int num = 0;
	uint8_t buf[320];
	while(!((uint8_t)GPIOB->IDR & 0b00010000));
	while((uint8_t)GPIOB->IDR & 0b00010000);
	//while(HAL_GPIO_ReadPin(VSYNC_PORT, VSYNC_PIN) == GPIO_PIN_RESET);
	//while(HAL_GPIO_ReadPin(VSYNC_PORT, VSYNC_PIN) == GPIO_PIN_SET);
	while(hg--){
			uint8_t*b=buf,*b2=buf;
			lg2=wg/4;
			while(lg2--){
				while((uint8_t)GPIOB->IDR & 0b00100000);
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);
				*b++=(uint8_t)GPIOC->IDR;
				while(!((uint8_t)GPIOB->IDR & 0b00100000));
				while((uint8_t)GPIOB->IDR & 0b00100000);
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
				*b++=(uint8_t)GPIOC->IDR;
				while(!((uint8_t)GPIOB->IDR & 0b00100000));
				while((uint8_t)GPIOB->IDR & 0b00100000);
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
				*b++=(uint8_t)GPIOC->IDR;
				while(!((uint8_t)GPIOB->IDR & 0b00100000));
				while((uint8_t)GPIOB->IDR & 0b00100000);
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
				*b++=(uint8_t)GPIOC->IDR;
				while(!((uint8_t)GPIOB->IDR & 0b00100000));
				while((uint8_t)GPIOB->IDR & 0b00100000);
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
				*b++=(uint8_t)GPIOC->IDR;
				image_buffer[hg][lg2] = *b2++;
				num++;
				//HAL_UART_Transmit(&huart2, b2++, sizeof(b2), HAL_MAX_DELAY);
				while(!((uint8_t)GPIOB->IDR & 0b00100000));
				//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
			}
			/* Finish sending the remainder during blanking */
			lg2=320-(wg/4);
			while(lg2--) {
				image_buffer[hg][lg2] = *b2++;
				num++;
				//HAL_UART_Transmit(&huart2, b2++, sizeof(b2), HAL_MAX_DELAY);
			}
		}
		//endOfLine();
		return num;
}

int camera_capture_image2(uint8_t image_buffer[][2*COLS]) {
	uint16_t hg = 120, wg = 320, lg2;
	int num = 0;
	while(!((uint8_t)GPIOB->IDR & 0b00010000));
	while((uint8_t)GPIOB->IDR & 0b00010000);
	//while(HAL_GPIO_ReadPin(VSYNC_PORT, VSYNC_PIN) == GPIO_PIN_RESET);
	//while(HAL_GPIO_ReadPin(VSYNC_PORT, VSYNC_PIN) == GPIO_PIN_SET);
	while(hg--){
		lg2=wg;
		while(lg2--){
			while((uint8_t)GPIOB->IDR & 0b00100000);
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			while((uint8_t)GPIOB->IDR & 0b00100000);
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			while((uint8_t)GPIOB->IDR & 0b00100000);
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			while((uint8_t)GPIOB->IDR & 0b00100000);
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			while((uint8_t)GPIOB->IDR & 0b00100000);
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_SET);//wait for low
			image_buffer[hg][lg2] = (uint8_t)GPIOC->IDR;
			num++;
			//HAL_UART_Transmit(&huart2, b2++, sizeof(b2), HAL_MAX_DELAY);
			while(!((uint8_t)GPIOB->IDR & 0b00100000));
			//while(HAL_GPIO_ReadPin(PCLK_PORT, PCLK_PIN) == GPIO_PIN_RESET);//wait for high
		}
	}
	//endOfLine();
	return num;
}
