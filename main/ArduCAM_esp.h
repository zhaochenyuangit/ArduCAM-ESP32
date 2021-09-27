#ifndef ArduCAM_H
#define ArduCAM_H

#include "stdint.h"
#include "driver/gpio.h"
#include "FreeRTOS/freertos.h"
#include "FreeRTOS/task.h"
#include "interface_spi.h"
#include "interface_i2c.h"

#define OV2640_MINI_2MP
#define OV2640_CAM
#define ESP32

#if defined(ESP32)
#define cbi(reg, bitmask) gpio_set_level((gpio_num_t)bitmask, 0)
#define sbi(reg, bitmask) gpio_set_level((gpio_num_t)bitmask, 1)
#define pulse_high(reg, bitmask) \
	sbi(reg, bitmask);           \
	cbi(reg, bitmask);
#define pulse_low(reg, bitmask) \
	cbi(reg, bitmask);          \
	sbi(reg, bitmask);

#define cport(port, data) port &= data
#define sport(port, data) port |= data

#define swap(type, i, j) \
	{                    \
		type t = i;      \
		i = j;           \
		j = t;           \
	}

#define fontbyte(x) cfont.font[x] //???

#define regtype volatile uint32_t
#define regsize uint32_t
#endif

#define pgm_read_byte(x) (*((char *)x))
#define pgm_read_word(x) (((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_byte_near(x) (*((char *)x))
#define pgm_read_byte_far(x) (*((char *)x))
#define pgm_read_word_near(x) (((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_word_far(x)    ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x))))

typedef unsigned char byte;
/****************************************************/
/* Sensor related definition 												*/
/****************************************************/
typedef enum
{
	BMP = 0,
	JPEG,
	RAW,
} image_format_t;

typedef enum
{
	OV7670 = 0,
	MT9D111_A,
	OV7675,
	OV5642,
	OV3640,
	OV2640,
	OV9655,
	MT9M112,
	OV7725,
	OV7660,
	MT9M001,
	OV5640,
	MT9D111_B,
	OV9650,
	MT9V111,
	MT9T112,
	MT9D112,
	MT9V034,
} arducam_camera_module_t;

typedef enum
{
	OV2640_160x120 = 0,
	OV2640_176x144,
	OV2640_320x240,
	OV2640_352x288,
	OV2640_640x480,
	OV2640_800x600,
	OV2640_1024x768,
	OV2640_1280x1024,
	OV2640_1600x1200,
} arducam_ov2640_resolution_t;

typedef enum
{
	OV5642_320x240 = 0,
	OV5642_640x480,
	OV5642_1024x768,
	OV5642_1280x960,
	OV5642_1600x1200,
	OV5642_2048x1536,
	OV5642_2592x1944,
	OV5642_1920x1080,
} arducam_ov5642_resolution_t;

typedef enum
{
	OV5640_320x240 = 0,
	OV5640_352x288,
	OV5640_640x480,
	OV5640_800x480,
	OV5640_1024x768,
	OV5640_1280x960,
	OV5640_1600x1200,
	OV5640_2048x1536,
	OV5640_2592x1944,
} arducam_ov5640_resolution_t;

typedef enum
{
	Auto = 0,
	Sunny,
	Cloudy,
	Office,
	Home,
} arducam_light_mode_t;

typedef enum
{
	Advanced_AWB = 0,
	Simple_AWB,
	Manual_day,
	Manual_A,
	Manual_cwf,
	Manual_cloudy,
} arducam_ov5642_light_mode_t;

typedef enum
{
	Saturation4 = 0,
	Saturation3,
	Saturation2,
	Saturation1,
	Saturation0,
	Saturation_1,
	Saturation_2,
	Saturation_3,
	Saturation_4,
} arducam_color_saturation_t;

typedef enum
{
	Brightness4 = 0,
	Brightness3,
	Brightness2,
	Brightness1,
	Brightness0,
	Brightness_1,
	Brightness_2,
	Brightness_3,
	Brightness_4,
} arducam_brightness_t;

typedef enum
{
	Contrast4 = 0,
	Contrast3,
	Contrast2,
	Contrast1,
	Contrast0,
	Contrast_1,
	Contrast_2,
	Contrast_3,
	Contrast_4,
} arducam_contrast_t;

typedef enum
{
	degree_180 = 0,
	degree_150,
	degree_120,
	degree_90,
	degree_60,
	degree_30,
	degree_0,
	degree30,
	degree60,
	degree90,
	degree120,
	degree150,
} arducam_degree_t;

typedef enum
{
	Antique = 0,
	Bluish,
	Greenish,
	Reddish,
	BW,
	Negative,
	BWnegative,
	Normal,
	Sepia,
	Overexposure,
	Solarize,
	Blueish,
	Yellowish,
} arducam_special_effects_t;

typedef enum
{
	Exposure_17_EV = 0,
	Exposure_13_EV,
	Exposure_10_EV,
	Exposure_07_EV,
	Exposure_03_EV,
	Exposure_default,
	Exposure07_EV,
	Exposure10_EV,
	Exposure13_EV,
	Exposure17_EV,
	Exposure03_EV,
} arducam_exposure_t;

typedef enum
{
	Auto_Sharpness_default = 0,
	Auto_Sharpness1,
	Auto_Sharpness2,
	Manual_Sharpnessoff,
	Manual_Sharpness1,
	Manual_Sharpness2,
	Manual_Sharpness3,
	Manual_Sharpness4,
	Manual_Sharpness5,
} arducam_ov5642_sharpness_mode_t;

typedef enum
{
	Sharpness1,
	Sharpness2,
	Sharpness3,
	Sharpness4,
	Sharpness5,
	Sharpness6,
	Sharpness7,
	Sharpness8,
	Sharpness_auto,
} arducam_ov3640_sharpness_mode_t;

typedef enum
{
	EV3 = 0,
	EV2,
	EV1,
	EV0,
	EV_1,
	EV_2,
	EV_3,
} arducam_exposure_value_t;

typedef enum
{
	MIRROR = 0,
	FLIP,
	MIRROR_FLIP,
} arducam_mirror_flip_t;

typedef enum
{
	high_quality = 0,
	default_quality,
	low_quality,
} arducam_quality_t;

typedef enum
{
	Color_bar = 0,
	Color_square,
	BW_square,
	DLI,
} arducam_pattern_t;

typedef enum
{
	Night_Mode_On = 0,
	Night_Mode_Off,
} arducam_night_mode_t;

typedef enum
{
	Off = 0,
	Manual_50HZ,
	Manual_60HZ,
	Auto_Detection,
} arducam_ov5640_banding_filter_t;

/****************************************************/
/* I2C Control Definition 													*/
/****************************************************/
#define I2C_ADDR_8BIT 0
#define I2C_ADDR_16BIT 1
#define I2C_REG_8BIT 0
#define I2C_REG_16BIT 1
#define I2C_DAT_8BIT 0
#define I2C_DAT_16BIT 1

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT 0xFF
#define SENSOR_REG_TERM_16BIT 0xFFFF
/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT 0xFF
#define SENSOR_VAL_TERM_16BIT 0xFFFF

//Define maximum frame buffer size
#if (defined OV2640_MINI_2MP)
#define MAX_FIFO_SIZE 0x5FFFF //384KByte
#elif (defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined ARDUCAM_SHIELD_REVC)
#define MAX_FIFO_SIZE 0x7FFFF //512KByte
#else
#define MAX_FIFO_SIZE 0x7FFFFF //8MByte
#endif

/****************************************************/
/* ArduChip registers definition 											*/
/****************************************************/
#define RWBIT 0x80 //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1 0x00 //TEST register

#if !(defined OV2640_MINI_2MP)
#define ARDUCHIP_FRAMES 0x01 //FRAME control register, Bit[2:0] = Number of frames to be captured																		//On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#endif

#define ARDUCHIP_MODE 0x02 //Mode register
#define MCU2LCD_MODE 0x00
#define CAM2LCD_MODE 0x01
#define LCD2MCU_MODE 0x02

#define ARDUCHIP_TIM 0x03 //Timming control
#if !(defined OV2640_MINI_2MP)
#define HREF_LEVEL_MASK 0x01  //0 = High active , 		1 = Low active
#define VSYNC_LEVEL_MASK 0x02 //0 = High active , 		1 = Low active
#define LCD_BKEN_MASK 0x04	  //0 = Enable, 					1 = Disable
#if (defined ARDUCAM_SHIELD_V2)
#define PCLK_REVERSE_MASK 0x08 //0 = Normal PCLK, 		1 = REVERSED PCLK
#else
#define PCLK_DELAY_MASK 0x08 //0 = data no delay,		1 = data delayed one PCLK
#endif
//#define MODE_MASK          		0x10  //0 = LCD mode, 				1 = FIFO mode
#endif
//#define FIFO_PWRDN_MASK	   		0x20  	//0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE			  0x40  	//0 = Normal mode, 			1 = Low power mode

#define ARDUCHIP_FIFO 0x04 //FIFO and I2C control
#define FIFO_CLEAR_MASK 0x01
#define FIFO_START_MASK 0x02
#define FIFO_RDPTR_RST_MASK 0x10
#define FIFO_WRPTR_RST_MASK 0x20

#define ARDUCHIP_GPIO 0x06 //GPIO Write Register
#if !(defined(ARDUCAM_SHIELD_V2) || defined(ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK 0x01 //0 = Sensor reset,							1 =  Sensor normal operation
#define GPIO_PWDN_MASK 0x02	 //0 = Sensor normal operation, 	1 = Sensor standby
#define GPIO_PWREN_MASK 0x04 //0 = Sensor LDO disable, 			1 = sensor LDO enable
#endif

#define BURST_FIFO_READ 0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ 0x3D //Single FIFO read operation

#define ARDUCHIP_REV 0x40 //ArduCHIP revision
#define VER_LOW_MASK 0x3F
#define VER_HIGH_MASK 0xC0

#define ARDUCHIP_TRIG 0x41 //Trigger source
#define VSYNC_MASK 0x01
#define SHUTTER_MASK 0x02
#define CAP_DONE_MASK 0x08

#define FIFO_SIZE1 0x42 //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2 0x43 //Camera write FIFO size[15:8]
#define FIFO_SIZE3 0x44 //Camera write FIFO size[18:16]

/****************************************************/

/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/
struct sensor_reg
{
	uint16_t reg;
	uint16_t val;
};

/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/

class ArduCAM
{
public:
	ArduCAM(gpio_num_t CS);
	virtual void InitCAM(void) = 0;

	virtual void set_JPEG_size(int size) = 0;
	virtual void set_Light_Mode(arducam_light_mode_t Light_Mode) = 0;
	virtual void set_Color_Saturation(arducam_color_saturation_t Color_Saturation) = 0;
	virtual void set_Brightness(arducam_brightness_t Brightness) = 0;
	virtual void set_Contrast(arducam_contrast_t Contrast) = 0;
	virtual void set_Special_effects(arducam_special_effects_t Special_effect) = 0;

	void CS_HIGH(void);
	void CS_LOW(void);

	void flush_fifo(void);
	void start_capture(void);
	void clear_fifo_flag(void);
	uint8_t read_fifo(void);

	uint8_t read_reg(uint8_t addr);
	void write_reg(uint8_t addr, uint8_t data);

	uint32_t read_fifo_length(void);
	void set_fifo_burst(void);

	void set_bit(uint8_t addr, uint8_t bit);
	void clear_bit(uint8_t addr, uint8_t bit);
	uint8_t get_bit(uint8_t addr, uint8_t bit);
	void set_mode(uint8_t mode);
	void set_format(byte fmt);

	uint8_t bus_write(int address, int value);
	uint8_t bus_read(int address);

	// Write 8 bit values to 8 bit register address
	int wrSensorRegs8_8(const struct sensor_reg *);

	// Write 16 bit values to 8 bit register address
	int wrSensorRegs8_16(const struct sensor_reg *);

	// Write 8 bit values to 16 bit register address
	int wrSensorRegs16_8(const struct sensor_reg *);

	// Write 16 bit values to 16 bit register address
	int wrSensorRegs16_16(const struct sensor_reg *);

	// Read/write 8 bit value to/from 8 bit register address
	byte wrSensorReg8_8(int regID, int regDat);
	byte rdSensorReg8_8(uint8_t regID, uint8_t *regDat);

	// Read/write 16 bit value to/from 8 bit register address
	byte wrSensorReg8_16(int regID, int regDat);
	byte rdSensorReg8_16(uint8_t regID, uint16_t *regDat);

	// Read/write 8 bit value to/from 16 bit register address
	byte wrSensorReg16_8(int regID, int regDat);
	byte rdSensorReg16_8(uint16_t regID, uint8_t *regDat);

	// Read/write 16 bit value to/from 16 bit register address
	byte wrSensorReg16_16(int regID, int regDat);
	byte rdSensorReg16_16(uint16_t regID, uint16_t *regDat);

protected:
	regtype *P_CS;
	regsize B_CS;
	uint8_t m_fmt;
	uint8_t sensor_model;
	uint8_t sensor_addr;
};

#endif