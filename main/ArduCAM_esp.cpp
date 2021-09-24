#include "ArduCAM_esp.h"

/**
 *  Arducam utility functions
 * */
uint8_t ArduCAM::read_reg(uint8_t addr)
{
	uint8_t data;
#if defined(RASPBERRY_PI)
	data = bus_read(addr);
#else
	data = bus_read(addr & 0x7F);
#endif
	return data;
}

void ArduCAM::write_reg(uint8_t addr, uint8_t data)
{
#if defined(RASPBERRY_PI)
	bus_write(addr, data);
#else
	bus_write(addr | 0x80, data);
#endif
}

//Set corresponding bit
void ArduCAM::set_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = read_reg(addr);
	write_reg(addr, temp | bit);
}
//Clear corresponding bit
void ArduCAM::clear_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = read_reg(addr);
	write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
uint8_t ArduCAM::get_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = read_reg(addr);
	temp = temp & bit;
	return temp;
}

/**
 *  Arducam API
 * */
ArduCAM::ArduCAM()
{
	sensor_model = OV7670;
	sensor_addr = 0x42;
}
ArduCAM::ArduCAM(uint8_t model, gpio_num_t CS)
{
	B_CS = CS;
	//printf("initialize camera, CS: %d\n",CS);
	gpio_set_direction(CS, GPIO_MODE_OUTPUT);
	gpio_pullup_en(CS);
	sbi(P_CS, B_CS);
	sensor_addr = 0x30;
}

void ArduCAM::InitCAM()
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	wrSensorReg8_8(0xff, 0x01);
	wrSensorReg8_8(0x12, 0x80);
	vTaskDelay(pdMS_TO_TICKS(100));
	if (m_fmt == JPEG)
	{
		wrSensorRegs8_8(OV2640_JPEG_INIT);
		wrSensorRegs8_8(OV2640_YUV422);
		wrSensorRegs8_8(OV2640_JPEG);
		wrSensorReg8_8(0xff, 0x01);
		wrSensorReg8_8(0x15, 0x00);
		wrSensorRegs8_8(OV2640_320x240_JPEG);
		//wrSensorReg8_8(0xff, 0x00);
		//wrSensorReg8_8(0x44, 0x32);
	}
	else
	{
		wrSensorRegs8_8(OV2640_QVGA);
	}
#endif
}

void ArduCAM::flush_fifo(void)
{
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void ArduCAM::start_capture(void)
{
	write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void ArduCAM::clear_fifo_flag(void)
{
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

uint32_t ArduCAM::read_fifo_length(void)
{
	uint32_t len1, len2, len3, length = 0;
	len1 = read_reg(FIFO_SIZE1);
	len2 = read_reg(FIFO_SIZE2);
	len3 = read_reg(FIFO_SIZE3) & 0x7f;
	length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;
}

void ArduCAM::CS_HIGH(void)
{
	sbi(P_CS, B_CS);
}
void ArduCAM::CS_LOW(void)
{
	cbi(P_CS, B_CS);
}

uint8_t ArduCAM::read_fifo(void)
{
	uint8_t data;
	data = bus_read(SINGLE_FIFO_READ);
	return data;
}

//Set ArduCAM working mode
//MCU2LCD_MODE: MCU writes the LCD screen GRAM
//CAM2LCD_MODE: Camera takes control of the LCD screen
//LCD2MCU_MODE: MCU read the LCD screen GRAM
void ArduCAM::set_mode(uint8_t mode)
{
	switch (mode)
	{
	case MCU2LCD_MODE:
		write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
		break;
	case CAM2LCD_MODE:
		write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
		break;
	case LCD2MCU_MODE:
		write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
		break;
	default:
		write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
		break;
	}
}

void ArduCAM::OV2640_set_JPEG_size(uint8_t size)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (size)
	{
	case OV2640_160x120:
		wrSensorRegs8_8(OV2640_160x120_JPEG);
		break;
	case OV2640_176x144:
		wrSensorRegs8_8(OV2640_176x144_JPEG);
		break;
	case OV2640_320x240:
		wrSensorRegs8_8(OV2640_320x240_JPEG);
		break;
	case OV2640_352x288:
		wrSensorRegs8_8(OV2640_352x288_JPEG);
		break;
	case OV2640_640x480:
		wrSensorRegs8_8(OV2640_640x480_JPEG);
		break;
	case OV2640_800x600:
		wrSensorRegs8_8(OV2640_800x600_JPEG);
		break;
	case OV2640_1024x768:
		wrSensorRegs8_8(OV2640_1024x768_JPEG);
		break;
	case OV2640_1280x1024:
		wrSensorRegs8_8(OV2640_1280x1024_JPEG);
		break;
	case OV2640_1600x1200:
		wrSensorRegs8_8(OV2640_1600x1200_JPEG);
		break;
	default:
		wrSensorRegs8_8(OV2640_320x240_JPEG);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_RAW_size(uint8_t size)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (size)
	{
	case OV5642_640x480:
		wrSensorRegs16_8(OV5642_1280x960_RAW);
		wrSensorRegs16_8(OV5642_640x480_RAW);
		break;
	case OV5642_1280x960:
		wrSensorRegs16_8(OV5642_1280x960_RAW);
		break;
	case OV5642_1920x1080:
		wrSensorRegs16_8(ov5642_RAW);
		wrSensorRegs16_8(OV5642_1920x1080_RAW);
		break;
	case OV5642_2592x1944:
		wrSensorRegs16_8(ov5642_RAW);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_JPEG_size(uint8_t size)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	uint8_t reg_val;

	switch (size)
	{
	case OV5642_320x240:
		wrSensorRegs16_8(ov5642_320x240);
		break;
	case OV5642_640x480:
		wrSensorRegs16_8(ov5642_640x480);
		break;
	case OV5642_1024x768:
		wrSensorRegs16_8(ov5642_1024x768);
		break;
	case OV5642_1280x960:
		wrSensorRegs16_8(ov5642_1280x960);
		break;
	case OV5642_1600x1200:
		wrSensorRegs16_8(ov5642_1600x1200);
		break;
	case OV5642_2048x1536:
		wrSensorRegs16_8(ov5642_2048x1536);
		break;
	case OV5642_2592x1944:
		wrSensorRegs16_8(ov5642_2592x1944);
		break;
	default:
		wrSensorRegs16_8(ov5642_320x240);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_JPEG_size(uint8_t size)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (size)
	{
	case OV5640_320x240:
		wrSensorRegs16_8(OV5640_QSXGA2QVGA);
		break;
	case OV5640_352x288:
		wrSensorRegs16_8(OV5640_QSXGA2CIF);
		break;
	case OV5640_640x480:
		wrSensorRegs16_8(OV5640_QSXGA2VGA);
		break;
	case OV5640_800x480:
		wrSensorRegs16_8(OV5640_QSXGA2WVGA);
		break;
	case OV5640_1024x768:
		wrSensorRegs16_8(OV5640_QSXGA2XGA);
		break;
	case OV5640_1280x960:
		wrSensorRegs16_8(OV5640_QSXGA2SXGA);
		break;
	case OV5640_1600x1200:
		wrSensorRegs16_8(OV5640_QSXGA2UXGA);
		break;
	case OV5640_2048x1536:
		wrSensorRegs16_8(OV5640_QSXGA2QXGA);
		break;
	case OV5640_2592x1944:
		wrSensorRegs16_8(OV5640_JPEG_QSXGA);
		break;
	default:
		//320x240
		wrSensorRegs16_8(OV5640_QSXGA2QVGA);
		break;
	}
#endif
}

void ArduCAM::set_format(byte fmt)
{
	if (fmt == BMP)
		m_fmt = BMP;
	else
		m_fmt = JPEG;
}

void ArduCAM::OV2640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (Light_Mode)
	{

	case Auto:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x00); //AWB on
		break;
	case Sunny:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x40); //AWB off
		wrSensorReg8_8(0xcc, 0x5e);
		wrSensorReg8_8(0xcd, 0x41);
		wrSensorReg8_8(0xce, 0x54);
		break;
	case Cloudy:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x40); //AWB off
		wrSensorReg8_8(0xcc, 0x65);
		wrSensorReg8_8(0xcd, 0x41);
		wrSensorReg8_8(0xce, 0x4f);
		break;
	case Office:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x40); //AWB off
		wrSensorReg8_8(0xcc, 0x52);
		wrSensorReg8_8(0xcd, 0x41);
		wrSensorReg8_8(0xce, 0x66);
		break;
	case Home:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x40); //AWB off
		wrSensorReg8_8(0xcc, 0x42);
		wrSensorReg8_8(0xcd, 0x3f);
		wrSensorReg8_8(0xce, 0x71);
		break;
	default:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0xc7, 0x00); //AWB on
		break;
	}
#endif
}
void ArduCAM::OV3640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Light_Mode)
	{

	case Auto:
		wrSensorReg16_8(0x332b, 0x00); //AWB auto, bit[3]:0,auto
		break;
	case Sunny:
		wrSensorReg16_8(0x332b, 0x08); //AWB off
		wrSensorReg16_8(0x33a7, 0x5e);
		wrSensorReg16_8(0x33a8, 0x40);
		wrSensorReg16_8(0x33a9, 0x46);
		break;
	case Cloudy:
		wrSensorReg16_8(0x332b, 0x08);
		wrSensorReg16_8(0x33a7, 0x68);
		wrSensorReg16_8(0x33a8, 0x40);
		wrSensorReg16_8(0x33a9, 0x4e);
		break;
	case Office:
		wrSensorReg16_8(0x332b, 0x08);
		wrSensorReg16_8(0x33a7, 0x52);
		wrSensorReg16_8(0x33a8, 0x40);
		wrSensorReg16_8(0x33a9, 0x58);
		break;
	case Home:
		wrSensorReg16_8(0x332b, 0x08);
		wrSensorReg16_8(0x33a7, 0x44);
		wrSensorReg16_8(0x33a8, 0x40);
		wrSensorReg16_8(0x33a9, 0x70);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Light_Mode(uint8_t Light_Mode)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (Light_Mode)
	{

	case Advanced_AWB:
		wrSensorReg16_8(0x3406, 0x0);
		wrSensorReg16_8(0x5192, 0x04);
		wrSensorReg16_8(0x5191, 0xf8);
		wrSensorReg16_8(0x518d, 0x26);
		wrSensorReg16_8(0x518f, 0x42);
		wrSensorReg16_8(0x518e, 0x2b);
		wrSensorReg16_8(0x5190, 0x42);
		wrSensorReg16_8(0x518b, 0xd0);
		wrSensorReg16_8(0x518c, 0xbd);
		wrSensorReg16_8(0x5187, 0x18);
		wrSensorReg16_8(0x5188, 0x18);
		wrSensorReg16_8(0x5189, 0x56);
		wrSensorReg16_8(0x518a, 0x5c);
		wrSensorReg16_8(0x5186, 0x1c);
		wrSensorReg16_8(0x5181, 0x50);
		wrSensorReg16_8(0x5184, 0x20);
		wrSensorReg16_8(0x5182, 0x11);
		wrSensorReg16_8(0x5183, 0x0);
		break;
	case Simple_AWB:
		wrSensorReg16_8(0x3406, 0x00);
		wrSensorReg16_8(0x5183, 0x80);
		wrSensorReg16_8(0x5191, 0xff);
		wrSensorReg16_8(0x5192, 0x00);
		break;
	case Manual_day:
		wrSensorReg16_8(0x3406, 0x1);
		wrSensorReg16_8(0x3400, 0x7);
		wrSensorReg16_8(0x3401, 0x32);
		wrSensorReg16_8(0x3402, 0x4);
		wrSensorReg16_8(0x3403, 0x0);
		wrSensorReg16_8(0x3404, 0x5);
		wrSensorReg16_8(0x3405, 0x36);
		break;
	case Manual_A:
		wrSensorReg16_8(0x3406, 0x1);
		wrSensorReg16_8(0x3400, 0x4);
		wrSensorReg16_8(0x3401, 0x88);
		wrSensorReg16_8(0x3402, 0x4);
		wrSensorReg16_8(0x3403, 0x0);
		wrSensorReg16_8(0x3404, 0x8);
		wrSensorReg16_8(0x3405, 0xb6);
		break;
	case Manual_cwf:
		wrSensorReg16_8(0x3406, 0x1);
		wrSensorReg16_8(0x3400, 0x6);
		wrSensorReg16_8(0x3401, 0x13);
		wrSensorReg16_8(0x3402, 0x4);
		wrSensorReg16_8(0x3403, 0x0);
		wrSensorReg16_8(0x3404, 0x7);
		wrSensorReg16_8(0x3405, 0xe2);
		break;
	case Manual_cloudy:
		wrSensorReg16_8(0x3406, 0x1);
		wrSensorReg16_8(0x3400, 0x7);
		wrSensorReg16_8(0x3401, 0x88);
		wrSensorReg16_8(0x3402, 0x4);
		wrSensorReg16_8(0x3403, 0x0);
		wrSensorReg16_8(0x3404, 0x5);
		wrSensorReg16_8(0x3405, 0x0);
		break;
	default:
		break;
	}
#endif
}

void ArduCAM::OV5640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (Light_Mode)
	{
	case Auto:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x00);
		wrSensorReg16_8(0x3400, 0x04);
		wrSensorReg16_8(0x3401, 0x00);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x04);
		wrSensorReg16_8(0x3405, 0x00);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		wrSensorReg16_8(0x5183, 0x0);
		break;
	case Sunny:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x01);
		wrSensorReg16_8(0x3400, 0x06);
		wrSensorReg16_8(0x3401, 0x1c);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x04);
		wrSensorReg16_8(0x3405, 0xf3);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		break;
	case Office:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x01);
		wrSensorReg16_8(0x3400, 0x05);
		wrSensorReg16_8(0x3401, 0x48);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x07);
		wrSensorReg16_8(0x3405, 0xcf);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x01);
		wrSensorReg16_8(0x3400, 0x06);
		wrSensorReg16_8(0x3401, 0x48);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x04);
		wrSensorReg16_8(0x3405, 0xd3);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		break;
	case Cloudy:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x01);
		wrSensorReg16_8(0x3400, 0x06);
		wrSensorReg16_8(0x3401, 0x48);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x04);
		wrSensorReg16_8(0x3405, 0xd3);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		break;
	case Home:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3406, 0x01);
		wrSensorReg16_8(0x3400, 0x04);
		wrSensorReg16_8(0x3401, 0x10);
		wrSensorReg16_8(0x3402, 0x04);
		wrSensorReg16_8(0x3403, 0x00);
		wrSensorReg16_8(0x3404, 0x08);
		wrSensorReg16_8(0x3405, 0x40);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
		break;
	default:
		break;
	}
#endif
}

void ArduCAM::OV2640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (Color_Saturation)
	{
	case Saturation2:

		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x02);
		wrSensorReg8_8(0x7c, 0x03);
		wrSensorReg8_8(0x7d, 0x68);
		wrSensorReg8_8(0x7d, 0x68);
		break;
	case Saturation1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x02);
		wrSensorReg8_8(0x7c, 0x03);
		wrSensorReg8_8(0x7d, 0x58);
		wrSensorReg8_8(0x7d, 0x58);
		break;
	case Saturation0:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x02);
		wrSensorReg8_8(0x7c, 0x03);
		wrSensorReg8_8(0x7d, 0x48);
		wrSensorReg8_8(0x7d, 0x48);
		break;
	case Saturation_1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x02);
		wrSensorReg8_8(0x7c, 0x03);
		wrSensorReg8_8(0x7d, 0x38);
		wrSensorReg8_8(0x7d, 0x38);
		break;
	case Saturation_2:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x02);
		wrSensorReg8_8(0x7c, 0x03);
		wrSensorReg8_8(0x7d, 0x28);
		wrSensorReg8_8(0x7d, 0x28);
		break;
	}
#endif
}
void ArduCAM::OV3640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Color_Saturation)
	{
	case Saturation2:
		wrSensorReg16_8(0x3302, 0xef); //bit[7]:1, enable SDE
		wrSensorReg16_8(0x3355, 0x02); //enable color saturation
		wrSensorReg16_8(0x3358, 0x70);
		wrSensorReg16_8(0x3359, 0x70);
		break;
	case Saturation1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x02);
		wrSensorReg16_8(0x3358, 0x50);
		wrSensorReg16_8(0x3359, 0x50);
		break;
	case Saturation0:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x02);
		wrSensorReg16_8(0x3358, 0x40);
		wrSensorReg16_8(0x3359, 0x40);
		break;
	case Saturation_1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x02);
		wrSensorReg16_8(0x3358, 0x30);
		wrSensorReg16_8(0x3359, 0x30);
		break;
	case Saturation_2:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x02);
		wrSensorReg16_8(0x3358, 0x20);
		wrSensorReg16_8(0x3359, 0x20);
		break;
	}

#endif
}

void ArduCAM::OV5640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (Color_Saturation)
	{
	case Saturation3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x2b);
		wrSensorReg16_8(0x5385, 0xab);
		wrSensorReg16_8(0x5386, 0xd6);
		wrSensorReg16_8(0x5387, 0xda);
		wrSensorReg16_8(0x5388, 0xd6);
		wrSensorReg16_8(0x5389, 0x04);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x24);
		wrSensorReg16_8(0x5385, 0x8f);
		wrSensorReg16_8(0x5386, 0xb3);
		wrSensorReg16_8(0x5387, 0xb6);
		wrSensorReg16_8(0x5388, 0xb3);
		wrSensorReg16_8(0x5389, 0x03);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x1f);
		wrSensorReg16_8(0x5385, 0x7a);
		wrSensorReg16_8(0x5386, 0x9a);
		wrSensorReg16_8(0x5387, 0x9c);
		wrSensorReg16_8(0x5388, 0x9a);
		wrSensorReg16_8(0x5389, 0x02);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation0:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x1a);
		wrSensorReg16_8(0x5385, 0x66);
		wrSensorReg16_8(0x5386, 0x80);
		wrSensorReg16_8(0x5387, 0x82);
		wrSensorReg16_8(0x5388, 0x80);
		wrSensorReg16_8(0x5389, 0x02);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation_1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x15);
		wrSensorReg16_8(0x5385, 0x52);
		wrSensorReg16_8(0x5386, 0x66);
		wrSensorReg16_8(0x5387, 0x68);
		wrSensorReg16_8(0x5388, 0x66);
		wrSensorReg16_8(0x5389, 0x02);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation_2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x10);
		wrSensorReg16_8(0x5385, 0x3d);
		wrSensorReg16_8(0x5386, 0x4d);
		wrSensorReg16_8(0x5387, 0x4e);
		wrSensorReg16_8(0x5388, 0x4d);
		wrSensorReg16_8(0x5389, 0x01);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Saturation_3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5381, 0x1c);
		wrSensorReg16_8(0x5382, 0x5a);
		wrSensorReg16_8(0x5383, 0x06);
		wrSensorReg16_8(0x5384, 0x0c);
		wrSensorReg16_8(0x5385, 0x30);
		wrSensorReg16_8(0x5386, 0x3d);
		wrSensorReg16_8(0x5387, 0x3e);
		wrSensorReg16_8(0x5388, 0x3d);
		wrSensorReg16_8(0x5389, 0x01);
		wrSensorReg16_8(0x538b, 0x98);
		wrSensorReg16_8(0x538a, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	}

#endif
}

void ArduCAM::OV5642_set_Color_Saturation(uint8_t Color_Saturation)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)

	switch (Color_Saturation)
	{
	case Saturation4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x80);
		wrSensorReg16_8(0x5584, 0x80);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x70);
		wrSensorReg16_8(0x5584, 0x70);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x60);
		wrSensorReg16_8(0x5584, 0x60);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x50);
		wrSensorReg16_8(0x5584, 0x50);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation0:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x40);
		wrSensorReg16_8(0x5584, 0x40);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation_1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x30);
		wrSensorReg16_8(0x5584, 0x30);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation_2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x20);
		wrSensorReg16_8(0x5584, 0x20);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation_3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x10);
		wrSensorReg16_8(0x5584, 0x10);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	case Saturation_4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5583, 0x00);
		wrSensorReg16_8(0x5584, 0x00);
		wrSensorReg16_8(0x5580, 0x02);
		break;
	}
#endif
}

void ArduCAM::OV2640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (Brightness)
	{
	case Brightness2:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x09);
		wrSensorReg8_8(0x7d, 0x40);
		wrSensorReg8_8(0x7d, 0x00);
		break;
	case Brightness1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x09);
		wrSensorReg8_8(0x7d, 0x30);
		wrSensorReg8_8(0x7d, 0x00);
		break;
	case Brightness0:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x09);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x00);
		break;
	case Brightness_1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x09);
		wrSensorReg8_8(0x7d, 0x10);
		wrSensorReg8_8(0x7d, 0x00);
		break;
	case Brightness_2:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x09);
		wrSensorReg8_8(0x7d, 0x00);
		wrSensorReg8_8(0x7d, 0x00);
		break;
	}
#endif
}
void ArduCAM::OV3640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Brightness)
	{
	case Brightness3:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04); //bit[2] enable
		wrSensorReg16_8(0x3354, 0x01); //bit[3] sign of brightness
		wrSensorReg16_8(0x335e, 0x30);
		break;
	case Brightness2:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335e, 0x20);
		break;
	case Brightness1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335e, 0x10);
		break;
	case Brightness0:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335e, 0x00);
		break;
	case Brightness_1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x09);
		wrSensorReg16_8(0x335e, 0x10);
		break;
	case Brightness_2:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x09);
		wrSensorReg16_8(0x335e, 0x20);
		break;
	case Brightness_3:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x09);
		wrSensorReg16_8(0x335e, 0x30);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Brightness(uint8_t Brightness)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)

	switch (Brightness)
	{
	case Brightness4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x40);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Brightness3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x30);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Brightness2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x20);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Brightness1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x10);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Brightness0:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x00);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Brightness_1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x10);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x08);
		break;
	case Brightness_2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x20);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x08);
		break;
	case Brightness_3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x30);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x08);
		break;
	case Brightness_4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5589, 0x40);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x558a, 0x08);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (Brightness)
	{
	case Brightness4:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x40);
		wrSensorReg16_8(0x5588, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x30);
		wrSensorReg16_8(0x5588, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x20);
		wrSensorReg16_8(0x5588, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x10);
		wrSensorReg16_8(0x5588, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness0:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x00);
		wrSensorReg16_8(0x5588, 0x01);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness_1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x10);
		wrSensorReg16_8(0x5588, 0x09);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness_2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x20);
		wrSensorReg16_8(0x5588, 0x09);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness_3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x30);
		wrSensorReg16_8(0x5588, 0x09);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Brightness_4:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5587, 0x40);
		wrSensorReg16_8(0x5588, 0x09);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	}
#endif
}

void ArduCAM::OV2640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (Contrast)
	{
	case Contrast2:

		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x07);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x28);
		wrSensorReg8_8(0x7d, 0x0c);
		wrSensorReg8_8(0x7d, 0x06);
		break;
	case Contrast1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x07);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x24);
		wrSensorReg8_8(0x7d, 0x16);
		wrSensorReg8_8(0x7d, 0x06);
		break;
	case Contrast0:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x07);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x06);
		break;
	case Contrast_1:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x07);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x2a);
		wrSensorReg8_8(0x7d, 0x06);
		break;
	case Contrast_2:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x04);
		wrSensorReg8_8(0x7c, 0x07);
		wrSensorReg8_8(0x7d, 0x20);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7d, 0x34);
		wrSensorReg8_8(0x7d, 0x06);
		break;
	}
#endif
}

void ArduCAM::OV3640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Contrast)
	{
	case Contrast3:

		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04); //bit[2] enable contrast/brightness
		wrSensorReg16_8(0x3354, 0x01); //bit[2] Yoffset sign
		wrSensorReg16_8(0x335c, 0x2c);
		wrSensorReg16_8(0x335d, 0x2c);
		break;
	case Contrast2:

		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x28);
		wrSensorReg16_8(0x335d, 0x28);
		break;
	case Contrast1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x24);
		wrSensorReg16_8(0x335d, 0x24);
		break;
	case Contrast0:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x20);
		wrSensorReg16_8(0x335d, 0x20);
		break;
	case Contrast_1:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x1c);
		wrSensorReg16_8(0x335d, 0x1c);
		break;
	case Contrast_2:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x18);
		wrSensorReg16_8(0x335d, 0x18);
		break;
	case Contrast_3:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x04);
		wrSensorReg16_8(0x3354, 0x01);
		wrSensorReg16_8(0x335c, 0x14);
		wrSensorReg16_8(0x335d, 0x14);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Contrast(uint8_t Contrast)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (Contrast)
	{
	case Contrast4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x30);
		wrSensorReg16_8(0x5588, 0x30);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x2c);
		wrSensorReg16_8(0x5588, 0x2c);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x28);
		wrSensorReg16_8(0x5588, 0x28);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x24);
		wrSensorReg16_8(0x5588, 0x24);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast0:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x20);
		wrSensorReg16_8(0x5588, 0x20);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast_1:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x1C);
		wrSensorReg16_8(0x5588, 0x1C);
		wrSensorReg16_8(0x558a, 0x1C);
		break;
	case Contrast_2:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x18);
		wrSensorReg16_8(0x5588, 0x18);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast_3:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x14);
		wrSensorReg16_8(0x5588, 0x14);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	case Contrast_4:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x04);
		wrSensorReg16_8(0x5587, 0x10);
		wrSensorReg16_8(0x5588, 0x10);
		wrSensorReg16_8(0x558a, 0x00);
		break;
	}
#endif
}
void ArduCAM::OV5640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (Contrast)
	{
	case Contrast3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x2c);
		wrSensorReg16_8(0x5585, 0x1c);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x28);
		wrSensorReg16_8(0x5585, 0x18);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x24);
		wrSensorReg16_8(0x5585, 0x10);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast0:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x20);
		wrSensorReg16_8(0x5585, 0x00);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast_1:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x1c);
		wrSensorReg16_8(0x5585, 0x1c);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast_2:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x18);
		wrSensorReg16_8(0x5585, 0x18);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Contrast_3:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5586, 0x14);
		wrSensorReg16_8(0x5585, 0x14);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	}
#endif
}

void ArduCAM::OV5642_set_hue(uint8_t degree)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (degree)
	{
	case degree_180:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x80);
		wrSensorReg16_8(0x5582, 0x00);
		wrSensorReg16_8(0x558a, 0x32);
		break;
	case degree_150:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x6f);
		wrSensorReg16_8(0x5582, 0x40);
		wrSensorReg16_8(0x558a, 0x32);
		break;
	case degree_120:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x40);
		wrSensorReg16_8(0x5582, 0x6f);
		wrSensorReg16_8(0x558a, 0x32);
		break;
	case degree_90:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x00);
		wrSensorReg16_8(0x5582, 0x80);
		wrSensorReg16_8(0x558a, 0x02);
		break;
	case degree_60:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x40);
		wrSensorReg16_8(0x5582, 0x6f);
		wrSensorReg16_8(0x558a, 0x02);
		break;
	case degree_30:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x6f);
		wrSensorReg16_8(0x5582, 0x40);
		wrSensorReg16_8(0x558a, 0x02);
		break;
	case degree_0:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x80);
		wrSensorReg16_8(0x5582, 0x00);
		wrSensorReg16_8(0x558a, 0x01);
		break;
	case degree30:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x6f);
		wrSensorReg16_8(0x5582, 0x40);
		wrSensorReg16_8(0x558a, 0x01);
		break;
	case degree60:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x40);
		wrSensorReg16_8(0x5582, 0x6f);
		wrSensorReg16_8(0x558a, 0x01);
		break;
	case degree90:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x00);
		wrSensorReg16_8(0x5582, 0x80);
		wrSensorReg16_8(0x558a, 0x31);
		break;
	case degree120:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x40);
		wrSensorReg16_8(0x5582, 0x6f);
		wrSensorReg16_8(0x558a, 0x31);
		break;
	case degree150:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x01);
		wrSensorReg16_8(0x5581, 0x6f);
		wrSensorReg16_8(0x5582, 0x40);
		wrSensorReg16_8(0x558a, 0x31);
		break;
	}
#endif
}

void ArduCAM::OV2640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP))
	switch (Special_effect)
	{
	case Antique:

		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x40);
		wrSensorReg8_8(0x7d, 0xa6);
		break;
	case Bluish:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0xa0);
		wrSensorReg8_8(0x7d, 0x40);
		break;
	case Greenish:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x40);
		wrSensorReg8_8(0x7d, 0x40);
		break;
	case Reddish:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x40);
		wrSensorReg8_8(0x7d, 0xc0);
		break;
	case BW:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x18);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x80);
		wrSensorReg8_8(0x7d, 0x80);
		break;
	case Negative:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x40);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x80);
		wrSensorReg8_8(0x7d, 0x80);
		break;
	case BWnegative:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x58);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x80);
		wrSensorReg8_8(0x7d, 0x80);

		break;
	case Normal:

		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x00);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x80);
		wrSensorReg8_8(0x7d, 0x80);

		break;
	}
#endif
}

void ArduCAM::OV3640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Special_effect)
	{
	case Antique:

		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18);
		wrSensorReg16_8(0x335a, 0x40);
		wrSensorReg16_8(0x335b, 0xa6);
		break;
	case Bluish:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18);
		wrSensorReg16_8(0x335a, 0xa0);
		wrSensorReg16_8(0x335b, 0x40);
		break;
	case Greenish:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18);
		wrSensorReg16_8(0x335a, 0x60);
		wrSensorReg16_8(0x335b, 0x60);
		break;
	case Reddish:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18);
		wrSensorReg16_8(0x335a, 0x80);
		wrSensorReg16_8(0x335b, 0xc0);
		break;
	case Yellowish:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18);
		wrSensorReg16_8(0x335a, 0x30);
		wrSensorReg16_8(0x335b, 0x90);
		break;
	case BW:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x18); //bit[4]fix u enable, bit[3]fix v enable
		wrSensorReg16_8(0x335a, 0x80);
		wrSensorReg16_8(0x335b, 0x80);
		break;
	case Negative:
		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x40); //bit[6] negative
		break;
	case BWnegative:
		wrSensorReg8_8(0xff, 0x00);
		wrSensorReg8_8(0x7c, 0x00);
		wrSensorReg8_8(0x7d, 0x58);
		wrSensorReg8_8(0x7c, 0x05);
		wrSensorReg8_8(0x7d, 0x80);
		wrSensorReg8_8(0x7d, 0x80);

		break;
	case Normal:

		wrSensorReg16_8(0x3302, 0xef);
		wrSensorReg16_8(0x3355, 0x00);

		break;
	}
#endif
}

void ArduCAM::OV5642_set_Special_effects(uint8_t Special_effect)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (Special_effect)
	{
	case Bluish:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x18);
		wrSensorReg16_8(0x5585, 0xa0);
		wrSensorReg16_8(0x5586, 0x40);
		break;
	case Greenish:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x18);
		wrSensorReg16_8(0x5585, 0x60);
		wrSensorReg16_8(0x5586, 0x60);
		break;
	case Reddish:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x18);
		wrSensorReg16_8(0x5585, 0x80);
		wrSensorReg16_8(0x5586, 0xc0);
		break;
	case BW:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x18);
		wrSensorReg16_8(0x5585, 0x80);
		wrSensorReg16_8(0x5586, 0x80);
		break;
	case Negative:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x40);
		break;

	case Sepia:
		wrSensorReg16_8(0x5001, 0xff);
		wrSensorReg16_8(0x5580, 0x18);
		wrSensorReg16_8(0x5585, 0x40);
		wrSensorReg16_8(0x5586, 0xa0);
		break;
	case Normal:
		wrSensorReg16_8(0x5001, 0x7f);
		wrSensorReg16_8(0x5580, 0x00);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (Special_effect)
	{
	case Normal:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x06);
		wrSensorReg16_8(0x5583, 0x40); // sat U
		wrSensorReg16_8(0x5584, 0x10); // sat V
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group
		break;
	case Blueish:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0xa0);
		wrSensorReg16_8(0x5584, 0x40);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Reddish:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0x80);
		wrSensorReg16_8(0x5584, 0xc0);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case BW:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0x80);
		wrSensorReg16_8(0x5584, 0x80);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Sepia:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0x40);
		wrSensorReg16_8(0x5584, 0xa0);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;

	case Negative:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x40);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x5583, 0x40); // sat U
		wrSensorReg16_8(0x5584, 0x10); // sat V
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Greenish:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0x60);
		wrSensorReg16_8(0x5584, 0x60);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Overexposure:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x1e);
		wrSensorReg16_8(0x5583, 0xf0);
		wrSensorReg16_8(0x5584, 0xf0);
		wrSensorReg16_8(0x5003, 0x08);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	case Solarize:
		wrSensorReg16_8(0x3212, 0x03); // start group 3
		wrSensorReg16_8(0x5580, 0x06);
		wrSensorReg16_8(0x5583, 0x40); // sat U
		wrSensorReg16_8(0x5584, 0x10); // sat V
		wrSensorReg16_8(0x5003, 0x09);
		wrSensorReg16_8(0x3212, 0x13); // end group 3
		wrSensorReg16_8(0x3212, 0xa3); // launch group 3
		break;
	}
#endif
}

void ArduCAM::OV3640_set_Exposure_level(uint8_t level)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (level)
	{
	case Exposure_17_EV:
		wrSensorReg16_8(0x3018, 0x10);
		wrSensorReg16_8(0x3019, 0x08);
		wrSensorReg16_8(0x301a, 0x21);
		break;
	case Exposure_13_EV:
		wrSensorReg16_8(0x3018, 0x18);
		wrSensorReg16_8(0x3019, 0x10);
		wrSensorReg16_8(0x301a, 0x31);
		break;
	case Exposure_10_EV:
		wrSensorReg16_8(0x3018, 0x20);
		wrSensorReg16_8(0x3019, 0x18);
		wrSensorReg16_8(0x301a, 0x41);
		break;
	case Exposure_07_EV:
		wrSensorReg16_8(0x3018, 0x28);
		wrSensorReg16_8(0x3019, 0x20);
		wrSensorReg16_8(0x301a, 0x51);
		break;
	case Exposure_03_EV:
		wrSensorReg16_8(0x3018, 0x30);
		wrSensorReg16_8(0x3019, 0x28);
		wrSensorReg16_8(0x301a, 0x61);
		break;
	case Exposure_default:
		wrSensorReg16_8(0x3018, 0x38);
		wrSensorReg16_8(0x3019, 0x30);
		wrSensorReg16_8(0x301a, 0x61);
		break;
	case Exposure03_EV:
		wrSensorReg16_8(0x3018, 0x40);
		wrSensorReg16_8(0x3019, 0x38);
		wrSensorReg16_8(0x301a, 0x71);
		break;
	case Exposure07_EV:
		wrSensorReg16_8(0x3018, 0x48);
		wrSensorReg16_8(0x3019, 0x40);
		wrSensorReg16_8(0x301a, 0x81);
		break;
	case Exposure10_EV:
		wrSensorReg16_8(0x3018, 0x50);
		wrSensorReg16_8(0x3019, 0x48);
		wrSensorReg16_8(0x301a, 0x91);
		break;
	case Exposure13_EV:
		wrSensorReg16_8(0x3018, 0x58);
		wrSensorReg16_8(0x3019, 0x50);
		wrSensorReg16_8(0x301a, 0x91);
		break;
	case Exposure17_EV:
		wrSensorReg16_8(0x3018, 0x60);
		wrSensorReg16_8(0x3019, 0x58);
		wrSensorReg16_8(0x301a, 0xa1);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Exposure_level(uint8_t level)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (level)
	{
	case Exposure_17_EV:
		wrSensorReg16_8(0x3a0f, 0x10);
		wrSensorReg16_8(0x3a10, 0x08);
		wrSensorReg16_8(0x3a1b, 0x10);
		wrSensorReg16_8(0x3a1e, 0x08);
		wrSensorReg16_8(0x3a11, 0x20);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure_13_EV:
		wrSensorReg16_8(0x3a0f, 0x18);
		wrSensorReg16_8(0x3a10, 0x10);
		wrSensorReg16_8(0x3a1b, 0x18);
		wrSensorReg16_8(0x3a1e, 0x10);
		wrSensorReg16_8(0x3a11, 0x30);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure_10_EV:
		wrSensorReg16_8(0x3a0f, 0x20);
		wrSensorReg16_8(0x3a10, 0x18);
		wrSensorReg16_8(0x3a11, 0x41);
		wrSensorReg16_8(0x3a1b, 0x20);
		wrSensorReg16_8(0x3a1e, 0x18);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure_07_EV:
		wrSensorReg16_8(0x3a0f, 0x28);
		wrSensorReg16_8(0x3a10, 0x20);
		wrSensorReg16_8(0x3a11, 0x51);
		wrSensorReg16_8(0x3a1b, 0x28);
		wrSensorReg16_8(0x3a1e, 0x20);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure_03_EV:
		wrSensorReg16_8(0x3a0f, 0x30);
		wrSensorReg16_8(0x3a10, 0x28);
		wrSensorReg16_8(0x3a11, 0x61);
		wrSensorReg16_8(0x3a1b, 0x30);
		wrSensorReg16_8(0x3a1e, 0x28);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure_default:
		wrSensorReg16_8(0x3a0f, 0x38);
		wrSensorReg16_8(0x3a10, 0x30);
		wrSensorReg16_8(0x3a11, 0x61);
		wrSensorReg16_8(0x3a1b, 0x38);
		wrSensorReg16_8(0x3a1e, 0x30);
		wrSensorReg16_8(0x3a1f, 0x10);
		wrSensorReg16_8(0x3a0f, 0x40);
		wrSensorReg16_8(0x3a10, 0x38);
		wrSensorReg16_8(0x3a11, 0x71);
		wrSensorReg16_8(0x3a1b, 0x40);
		wrSensorReg16_8(0x3a1e, 0x38);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case Exposure07_EV:
		wrSensorReg16_8(0x3a0f, 0x48);
		wrSensorReg16_8(0x3a10, 0x40);
		wrSensorReg16_8(0x3a11, 0x80);
		wrSensorReg16_8(0x3a1b, 0x48);
		wrSensorReg16_8(0x3a1e, 0x40);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	case Exposure10_EV:
		wrSensorReg16_8(0x3a0f, 0x50);
		wrSensorReg16_8(0x3a10, 0x48);
		wrSensorReg16_8(0x3a11, 0x90);
		wrSensorReg16_8(0x3a1b, 0x50);
		wrSensorReg16_8(0x3a1e, 0x48);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	case Exposure13_EV:
		wrSensorReg16_8(0x3a0f, 0x58);
		wrSensorReg16_8(0x3a10, 0x50);
		wrSensorReg16_8(0x3a11, 0x91);
		wrSensorReg16_8(0x3a1b, 0x58);
		wrSensorReg16_8(0x3a1e, 0x50);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	case Exposure17_EV:
		wrSensorReg16_8(0x3a0f, 0x60);
		wrSensorReg16_8(0x3a10, 0x58);
		wrSensorReg16_8(0x3a11, 0xa0);
		wrSensorReg16_8(0x3a1b, 0x60);
		wrSensorReg16_8(0x3a1e, 0x58);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	}
#endif
}

void ArduCAM::OV3640_set_Sharpness(uint8_t Sharpness)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Sharpness)
	{
	case Sharpness1:
		wrSensorReg16_8(0x332d, 0x41);
		break;
	case Sharpness2:
		wrSensorReg16_8(0x332d, 0x42);
		break;
	case Sharpness3:
		wrSensorReg16_8(0x332d, 0x43);
		break;
	case Sharpness4:
		wrSensorReg16_8(0x332d, 0x44);
		break;
	case Sharpness5:
		wrSensorReg16_8(0x332d, 0x45);
		break;
	case Sharpness6:
		wrSensorReg16_8(0x332d, 0x46);
		break;
	case Sharpness7:
		wrSensorReg16_8(0x332d, 0x47);
		break;
	case Sharpness8:
		wrSensorReg16_8(0x332d, 0x48);
		break;
	case Sharpness_auto:
		wrSensorReg16_8(0x332d, 0x60);
		wrSensorReg16_8(0x332f, 0x03);
		break;
	}
#endif
}

void ArduCAM::OV3640_set_Mirror_Flip(uint8_t Mirror_Flip)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch (Mirror_Flip)
	{
	case MIRROR:
		wrSensorReg16_8(0x307c, 0x12); //mirror
		wrSensorReg16_8(0x3090, 0xc8);
		wrSensorReg16_8(0x3023, 0x0a);

		break;
	case FLIP:
		wrSensorReg16_8(0x307c, 0x11); //flip
		wrSensorReg16_8(0x3023, 0x09);
		wrSensorReg16_8(0x3090, 0xc0);
		break;
	case MIRROR_FLIP:
		wrSensorReg16_8(0x307c, 0x13); //flip/mirror
		wrSensorReg16_8(0x3023, 0x09);
		wrSensorReg16_8(0x3090, 0xc8);
		break;
	case Normal:
		wrSensorReg16_8(0x307c, 0x10); //no mirror/flip
		wrSensorReg16_8(0x3090, 0xc0);
		wrSensorReg16_8(0x3023, 0x0a);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Sharpness(uint8_t Sharpness)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (Sharpness)
	{
	case Auto_Sharpness_default:
		wrSensorReg16_8(0x530A, 0x00);
		wrSensorReg16_8(0x530c, 0x0);
		wrSensorReg16_8(0x530d, 0xc);
		wrSensorReg16_8(0x5312, 0x40);
		break;
	case Auto_Sharpness1:
		wrSensorReg16_8(0x530A, 0x00);
		wrSensorReg16_8(0x530c, 0x4);
		wrSensorReg16_8(0x530d, 0x18);
		wrSensorReg16_8(0x5312, 0x20);
		break;
	case Auto_Sharpness2:
		wrSensorReg16_8(0x530A, 0x00);
		wrSensorReg16_8(0x530c, 0x8);
		wrSensorReg16_8(0x530d, 0x30);
		wrSensorReg16_8(0x5312, 0x10);
		break;
	case Manual_Sharpnessoff:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x00);
		wrSensorReg16_8(0x531f, 0x00);
		break;
	case Manual_Sharpness1:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x04);
		wrSensorReg16_8(0x531f, 0x04);
		break;
	case Manual_Sharpness2:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x08);
		wrSensorReg16_8(0x531f, 0x08);
		break;
	case Manual_Sharpness3:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x0c);
		wrSensorReg16_8(0x531f, 0x0c);
		break;
	case Manual_Sharpness4:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x0f);
		wrSensorReg16_8(0x531f, 0x0f);
		break;
	case Manual_Sharpness5:
		wrSensorReg16_8(0x530A, 0x08);
		wrSensorReg16_8(0x531e, 0x1f);
		wrSensorReg16_8(0x531f, 0x1f);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Mirror_Flip(uint8_t Mirror_Flip)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	uint8_t reg_val;
	switch (Mirror_Flip)
	{
	case MIRROR:
		rdSensorReg16_8(0x3818, &reg_val);
		reg_val = reg_val | 0x00;
		reg_val = reg_val & 0x9F;
		wrSensorReg16_8(0x3818, reg_val);
		rdSensorReg16_8(0x3621, &reg_val);
		reg_val = reg_val | 0x20;
		wrSensorReg16_8(0x3621, reg_val);

		break;
	case FLIP:
		rdSensorReg16_8(0x3818, &reg_val);
		reg_val = reg_val | 0x20;
		reg_val = reg_val & 0xbF;
		wrSensorReg16_8(0x3818, reg_val);
		rdSensorReg16_8(0x3621, &reg_val);
		reg_val = reg_val | 0x20;
		wrSensorReg16_8(0x3621, reg_val);
		break;
	case MIRROR_FLIP:
		rdSensorReg16_8(0x3818, &reg_val);
		reg_val = reg_val | 0x60;
		reg_val = reg_val & 0xFF;
		wrSensorReg16_8(0x3818, reg_val);
		rdSensorReg16_8(0x3621, &reg_val);
		reg_val = reg_val & 0xdf;
		wrSensorReg16_8(0x3621, reg_val);
		break;
	case Normal:
		rdSensorReg16_8(0x3818, &reg_val);
		reg_val = reg_val | 0x40;
		reg_val = reg_val & 0xdF;
		wrSensorReg16_8(0x3818, reg_val);
		rdSensorReg16_8(0x3621, &reg_val);
		reg_val = reg_val & 0xdf;
		wrSensorReg16_8(0x3621, reg_val);
		break;
	}
#endif
}

void ArduCAM::OV5642_set_Compress_quality(uint8_t quality)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (quality)
	{
	case high_quality:
		wrSensorReg16_8(0x4407, 0x02);
		break;
	case default_quality:
		wrSensorReg16_8(0x4407, 0x04);
		break;
	case low_quality:
		wrSensorReg16_8(0x4407, 0x08);
		break;
	}
#endif
}

void ArduCAM::OV5642_Test_Pattern(uint8_t Pattern)
{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS)
	switch (Pattern)
	{
	case Color_bar:
		wrSensorReg16_8(0x503d, 0x80);
		wrSensorReg16_8(0x503e, 0x00);
		break;
	case Color_square:
		wrSensorReg16_8(0x503d, 0x85);
		wrSensorReg16_8(0x503e, 0x12);
		break;
	case BW_square:
		wrSensorReg16_8(0x503d, 0x85);
		wrSensorReg16_8(0x503e, 0x1a);
		break;
	case DLI:
		wrSensorReg16_8(0x4741, 0x4);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_Night_Mode(uint8_t Night_mode)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	uint8_t reg_val;
	switch (Night_mode)
	{
	case Night_Mode_On:
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val | 0x04;
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	case Night_Mode_Off:
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val & 0xfb;
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_Banding_Filter(uint8_t Banding_Filter)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	uint8_t reg_val;
	switch (Banding_Filter)
	{
	case Off:
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val & 0xdf; // turn off banding filter
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	case Manual_50HZ:
		wrSensorReg16_8(0x3c00, 04); // set to 50Hz
		wrSensorReg16_8(0x3c01, 80); // manual banding filter
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val | 0x20; // turn on banding filter
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	case Manual_60HZ:
		wrSensorReg16_8(0x3c00, 00); // set to 60Hz
		wrSensorReg16_8(0x3c01, 80); // manual banding filter
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val | 0x20; // turn on banding filter
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	case Auto_Detection:
		wrSensorReg16_8(0x3c01, 00); // auto banding filter
		rdSensorReg16_8(0x3a00, &reg_val);
		reg_val = reg_val & 0xdf; // turn off banding filter
		wrSensorReg16_8(0x3a00, reg_val);
		break;
	}
#endif
}

void ArduCAM::OV5640_set_EV(uint8_t EV)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (EV)
	{
	case EV3:
		wrSensorReg16_8(0x3a0f, 0x60);
		wrSensorReg16_8(0x3a10, 0x58);
		wrSensorReg16_8(0x3a11, 0xa0);
		wrSensorReg16_8(0x3a1b, 0x60);
		wrSensorReg16_8(0x3a1e, 0x58);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	case EV2:
		wrSensorReg16_8(0x3a0f, 0x50);
		wrSensorReg16_8(0x3a10, 0x48);
		wrSensorReg16_8(0x3a11, 0x90);
		wrSensorReg16_8(0x3a1b, 0x50);
		wrSensorReg16_8(0x3a1e, 0x48);
		wrSensorReg16_8(0x3a1f, 0x20);
		break;
	case EV1:
		wrSensorReg16_8(0x3a0f, 0x40);
		wrSensorReg16_8(0x3a10, 0x38);
		wrSensorReg16_8(0x3a11, 0x71);
		wrSensorReg16_8(0x3a1b, 0x40);
		wrSensorReg16_8(0x3a1e, 0x38);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case EV0:
		wrSensorReg16_8(0x3a0f, 0x38);
		wrSensorReg16_8(0x3a10, 0x30);
		wrSensorReg16_8(0x3a11, 0x61);
		wrSensorReg16_8(0x3a1b, 0x38);
		wrSensorReg16_8(0x3a1e, 0x30);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case EV_1:
		wrSensorReg16_8(0x3a0f, 0x30);
		wrSensorReg16_8(0x3a10, 0x28);
		wrSensorReg16_8(0x3a11, 0x61);
		wrSensorReg16_8(0x3a1b, 0x30);
		wrSensorReg16_8(0x3a1e, 0x28);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case EV_2:
		wrSensorReg16_8(0x3a0f, 0x20);
		wrSensorReg16_8(0x3a10, 0x18);
		wrSensorReg16_8(0x3a11, 0x41);
		wrSensorReg16_8(0x3a1b, 0x20);
		wrSensorReg16_8(0x3a1e, 0x18);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	case EV_3:
		wrSensorReg16_8(0x3a0f, 0x10);
		wrSensorReg16_8(0x3a10, 0x08);
		wrSensorReg16_8(0x3a1b, 0x10);
		wrSensorReg16_8(0x3a1e, 0x08);
		wrSensorReg16_8(0x3a11, 0x20);
		wrSensorReg16_8(0x3a1f, 0x10);
		break;
	}
#endif
}

// Write 8 bit values to 8 bit register address
int ArduCAM::wrSensorRegs8_8(const struct sensor_reg reglist[])
{
#if defined(RASPBERRY_PI)
	arducam_i2c_write_regs(reglist);
#else
	int err = 0;
	uint16_t reg_addr = 0;
	uint16_t reg_val = 0;
	const struct sensor_reg *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff))
	{
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
		err = wrSensorReg8_8(reg_addr, reg_val);
		next++;
#if (defined(ESP8266) || defined(ESP32))
		vTaskDelay(10);
#endif
	}
#endif
	return 1;
}

#ifndef OV2640_MINI_2MP
// Write 16 bit values to 8 bit register address
int ArduCAM::wrSensorRegs8_16(const struct sensor_reg reglist[])
{
#if defined(RASPBERRY_PI)
	arducam_i2c_write_regs16(reglist);
#else
	int err = 0;
	unsigned int reg_addr, reg_val;
	const struct sensor_reg *next = reglist;

	while ((reg_addr != 0xff) | (reg_val != 0xffff))
	{
#if defined(RASPBERRY_PI)
		reg_addr = next->reg;
		reg_val = next->val;
#else
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
#endif
		err = wrSensorReg8_16(reg_addr, reg_val);
		next++;
#if defined(ESP8266) || defined(ESP32)
		yield();
#endif
	}
#endif
	return 1;
}

// Write 8 bit values to 16 bit register address
int ArduCAM::wrSensorRegs16_8(const struct sensor_reg reglist[])
{
#if defined(RASPBERRY_PI)
	arducam_i2c_write_word_regs(reglist);
#else
	int err = 0;
	unsigned int reg_addr;
	unsigned char reg_val;
	const struct sensor_reg *next = reglist;

	while ((reg_addr != 0xffff) | (reg_val != 0xff))
	{

#if defined(RASPBERRY_PI)
		reg_addr = next->reg;
		reg_val = next->val;
#else
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
#endif
		err = wrSensorReg16_8(reg_addr, reg_val);
		//if (!err)
		//return err;
		next++;
#if defined(ESP8266) || defined(ESP32)
		yield();
#endif
	}
#endif
	return 1;
}

//I2C Array Write 16bit address, 16bit data
int ArduCAM::wrSensorRegs16_16(const struct sensor_reg reglist[])
{
#if defined(RASPBERRY_PI)
#else
	int err = 0;
	unsigned int reg_addr, reg_val;
	const struct sensor_reg *next = reglist;
	reg_addr = pgm_read_word(&next->reg);
	reg_val = pgm_read_word(&next->val);
	while ((reg_addr != 0xffff) | (reg_val != 0xffff))
	{
		err = wrSensorReg16_16(reg_addr, reg_val);
		//if (!err)
		//   return err;
		next++;
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
#if defined(ESP8266) || defined(ESP32)
		yield();
#endif
	}
#endif
	return 1;
}
#endif

/** 
 * Arudcam interface ESP32 modification
 * */

/** 
 * I2C interface
 * */
// Read/write 8 bit value to/from 8 bit register address
byte ArduCAM::wrSensorReg8_8(int regID, int regDat)
{
	uint8_t data = regDat & 0xff;
	esp_err_t err = i2c_master_write_slave(sensor_addr, (regID & 0xff), &data, 1);
	return (err == ESP_OK) ? 0 : 1;
}

byte ArduCAM::rdSensorReg8_8(uint8_t regID, uint8_t *regDat)
{
	esp_err_t err = i2c_master_read_slave(sensor_addr, regID, regDat, 1);
	return (err == ESP_OK) ? 0 : 1;
}
// Read/write 16 bit value to/from 8 bit register address
byte ArduCAM::wrSensorReg8_16(int regID, int regDat)
{
	uint8_t data[2];
	data[0] = regDat >> 8;
	data[1] = regDat & 0x00FF;
	esp_err_t err = i2c_master_write_slave(sensor_addr, (regID & 0xff), data, 2);
	return (err == ESP_OK) ? 0 : 1;
}

byte ArduCAM::rdSensorReg8_16(uint8_t regID, uint16_t *regDat)
{
	uint8_t data[2];
	esp_err_t err = i2c_master_read_slave(sensor_addr, regID, data, 2);
	*regDat = (data[0] << 8) | (data[1]);
	return (err == ESP_OK) ? 0 : 1;
}

/** 
 * SPI interface
 * */
uint8_t ArduCAM::bus_write(int address, int value)
{
	cbi(P_CS, B_CS);
	spi_transfer_reg((uint8_t)address,(uint8_t)value);
	sbi(P_CS, B_CS);
	
	return 0;
}

uint8_t ArduCAM::bus_read(int address)
{
	cbi(P_CS, B_CS);
	uint8_t value = spi_transfer_reg((uint8_t)address, 0x00);
	sbi(P_CS, B_CS);
	return value;
}

void ArduCAM::set_fifo_burst()
{
	spi_transfer_8(BURST_FIFO_READ);
}