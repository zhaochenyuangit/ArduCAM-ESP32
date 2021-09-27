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
ArduCAM::ArduCAM(gpio_num_t CS)
{
	B_CS = CS;
	gpio_set_direction(CS, GPIO_MODE_OUTPUT);
	gpio_pullup_en(CS);
	sbi(P_CS, B_CS);
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

void ArduCAM::set_format(byte fmt)
{
	if (fmt == BMP)
		m_fmt = BMP;
	else
		m_fmt = JPEG;
}

/** 
 * Arudcam interface ESP32 modification
 * */

/** 
 * I2C interface
 * */
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
	spi_transfer_reg((uint8_t)address, (uint8_t)value);
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