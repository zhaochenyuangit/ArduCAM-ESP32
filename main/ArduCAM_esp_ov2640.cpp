#include "ArduCAM_esp_ov2640.h"

static const char *TAG = "ArduCAM_OV2640";

void ArduCAM_OV2640::InitCAM()
{
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
}
void ArduCAM_OV2640::set_JPEG_size(int size = OV2640_320x240)
{
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
}

void ArduCAM_OV2640::set_Light_Mode(arducam_light_mode_t Light_Mode = Auto)
{
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
}

void ArduCAM_OV2640::set_Color_Saturation(arducam_color_saturation_t Color_Saturation)
{
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
    default:
        ESP_LOGW(TAG, "setting value not supported on this module, skip");
        break;
    }
}

void ArduCAM_OV2640::set_Brightness(arducam_brightness_t Brightness)
{
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
    default:
        ESP_LOGW(TAG, "setting value not supported on this module, skip");
        break;
    }
}

void ArduCAM_OV2640::set_Contrast(arducam_contrast_t Contrast)
{
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
    default:
        ESP_LOGW(TAG, "setting value not supported on this module, skip");
        break;
    }
}

void ArduCAM_OV2640::set_Special_effects(arducam_special_effects_t Special_effect)
{
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
    default:
        ESP_LOGW(TAG, "setting value not supported on this module, skip");
        break;
    }
}