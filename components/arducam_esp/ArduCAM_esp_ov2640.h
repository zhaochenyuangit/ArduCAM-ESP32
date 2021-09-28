#include "ArduCAM_esp.h"
#include "ov2640_regs.h"
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
