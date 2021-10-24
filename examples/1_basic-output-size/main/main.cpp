#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "time.h"
#include <sys/time.h>
#include "ArduCAM_esp_ov2640.h"
#include "mbedtls/base64.h"
#include "esp_heap_caps.h"

#include "interface_i2c.h"
#include "interface_spi.h"

#define BASE64_BUF_SIZE ((SPI_MAX_TRANS_SIZE / 3 + 2) * 4)
#define CS_PIN GPIO_NUM_5

ArduCAM_OV2640 myCAM(CS_PIN);

void OV2640_valid_check()
{
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
        printf("SPI: interface Error 0x%2x!\n", temp);
    else
        printf("SPI: interface OK\n");

    //Check if the camera module type is OV2640
    uint8_t vid, pid;
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
        printf("I2C: Can't find OV2640 module!\n");
    else
        printf("I2C: OV2640 detected.\n");
}

void capture_one_frame()
{
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        vTaskDelay(pdMS_TO_TICKS(2));
}

void read_frame_buffer(uint8_t *buffer)
{
    static size_t frame_num = 0;
    size_t len = myCAM.read_fifo_length();
    if (len >= MAX_FIFO_SIZE)
    {
        printf("Over size.\n");
        return;
    }
    else if (len == 0)
    {
        printf("Size is 0.\n");
        return;
    }
    size_t transfer_size = (len < SPI_MAX_TRANS_SIZE) ? len : SPI_MAX_TRANS_SIZE;
    myCAM.CS_LOW();
    spi_transfer_bytes(BURST_FIFO_READ, buffer, buffer, transfer_size);
    myCAM.CS_HIGH();
    printf("frame %d captured image! size: %d, first four bytes are: ", ++frame_num, len);
    for (int i = 0; i < 4; i++)
    {
        printf("%2x ", buffer[i]);
    }
    printf("\n");
}

void take_image(void *_)
{
    uint8_t *buffer = (uint8_t *)heap_caps_malloc(SPI_MAX_TRANS_SIZE, MALLOC_CAP_DMA);
    assert(buffer);
    while (1)
    {
        capture_one_frame();
        read_frame_buffer(buffer);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    free(buffer);
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(spi_master_init());
    ESP_ERROR_CHECK(i2c_master_init());

    OV2640_valid_check();
    //Change to JPEG capture mode and initialize the OV2640 module
    printf("initialize camera, may take up to one minute\n");
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.set_JPEG_size(OV2640_320x240);
    myCAM.clear_fifo_flag();

    xTaskCreatePinnedToCore(take_image, "take_image", 4096, NULL, 10, NULL, 1);
}
