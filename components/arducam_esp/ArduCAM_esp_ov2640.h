#ifndef ARDUCAM_OV2640
#define ARDUCAM_OV2640

#include "ArduCAM_esp.h"
#include "ov2640_regs.h"

#define ARDUCHIP_FRAMES			0x01
#define VSYNC_LEVEL_MASK   		0x02
#define FRAMES_NUM 				0x04

#ifdef __cplusplus
class ArduCAM_OV2640 : public ArduCAM
{
public:
    ArduCAM_OV2640(gpio_num_t cs_pin) : ArduCAM(cs_pin) { sensor_addr = 0x30; }
    void InitCAM();
    void set_JPEG_size(int size);
    void set_Light_Mode(arducam_light_mode_t Light_Mode);
    void set_Color_Saturation(arducam_color_saturation_t Color_Saturation);
    void set_Brightness(arducam_brightness_t Brightness);
    void set_Contrast(arducam_contrast_t Contrast);
    void set_Special_effects(arducam_special_effects_t Special_effect);
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct ArduCAM_OV2640_C ArduCAM_OV2640_C;

	ArduCAM_OV2640_C *myCAM_C;

	// wrapper functions for C compatibility, should this be moved to header file?
	extern void ArduCAM_set_bit(uint8_t addr, uint8_t bit);
	extern void *ArduCAM_OV2640_init(gpio_num_t CS_PIN);
	extern void ArduCAM_write_reg(uint8_t addr, uint8_t data);
	extern uint8_t ArduCAM_read_reg(uint8_t addr);
	extern byte ArduCAM_wrSensorReg8_8(int regID, int regDat);
	extern byte ArduCAM_rdSensorReg8_8(uint8_t regID, uint8_t *regDat);
	extern void ArduCAM_flush_fifo();
	extern void ArduCAM_clear_fifo_flag();
	extern void ArduCAM_start_capture();
	extern uint8_t ArduCAM_get_bit(uint8_t addr, uint8_t bit);
	extern uint32_t ArduCAM_read_fifo_length();
	extern void ArduCAM_CS_LOW();
	extern void ArduCAM_CS_HIGH();
	extern void ArduCAM_OV2640_InitCAM();
	extern void ArduCAM_set_format(byte fmt);
	extern void ArduCAM_OV2640_set_JPEG_size(int size);

#ifdef __cplusplus
}
#endif

#endif
