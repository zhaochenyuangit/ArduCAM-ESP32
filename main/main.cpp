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
#include "ArduCAM_esp.h"

extern "C"
{
#include "ArduCAM_I2C.h"
#include "ArduCAM_SPI.h"
#include "network_common.h"
#include "mbedtls/base64.h"
#include "math.h"
#include "esp_heap_caps.h"
}

#define CS_PIN GPIO_NUM_5

static esp_mqtt_client_handle_t client = NULL;
static SemaphoreHandle_t sema_capture_frame = NULL;
static SemaphoreHandle_t sema_read_frame = NULL;

ArduCAM myCAM(OV2640, CS_PIN);

void image_encode_base64(uint8_t *buf, size_t len)
{
    static uint8_t base64_buf[SPI_MAX_TRANS_SIZE / 3 * 4];
    size_t encode_len;

    mbedtls_base64_encode(base64_buf, SPI_MAX_TRANS_SIZE / 3 * 4, &encode_len, buf, len);
    printf("raw data len %d encode len %d\n", len, encode_len);
    //mqtt_send(client, "ov2640/base64", (char *)base64_buf);
    base64_buf[30] = '\0';
    printf("%s", base64_buf);
}

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
    printf("start capture\n");
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
    struct timeval tik, tok;
    gettimeofday(&tik, NULL);
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        vTaskDelay(pdMS_TO_TICKS(10));
    gettimeofday(&tok, NULL);
    int64_t time_ms = (int64_t)(tok.tv_sec - tik.tv_sec) * 1000 + (tok.tv_usec - tik.tv_usec) / 1000;
    printf("capture total_time used (in miliseconds): %lld\n", time_ms);
}

void read_frame_buffer(uint8_t *buffer)
{
    size_t len = myCAM.read_fifo_length();
    if (len >= 0x07ffff)
    {
        printf("Over size.\n");
        return;
    }
    else if (len == 0)
    {
        printf("Size is 0.\n");
        return;
    }
    printf("camcapture fifo length: %d\n", len);
    while (len > 0)
    {
        printf("remaining len: %d\n", len);
        size_t n_byte_read_in = (len < SPI_MAX_TRANS_SIZE) ? len : SPI_MAX_TRANS_SIZE;
        len -= n_byte_read_in;
        myCAM.CS_LOW();
        spi_transfer_bytes(BURST_FIFO_READ, buffer, buffer, n_byte_read_in);
        myCAM.CS_HIGH();
        for (int row = 0; row < 5; row++)
        {
            for (int col = 0; col < 5; col++)
            {
                printf("%2x ", buffer[row * 5 + col]);
            }
            printf("\n");
        }
        printf("\n");
        vTaskDelay(10);
    }
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
    //ESP_ERROR_CHECK(start_wifi());
    //ESP_ERROR_CHECK(start_mqtt(&client, MYMQTT, NULL, NULL));
    ESP_ERROR_CHECK(spi_init());
    ESP_ERROR_CHECK(i2c_master_init());

    OV2640_valid_check();
    //Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_320x240);
    myCAM.clear_fifo_flag();
    xTaskCreatePinnedToCore(take_image, "take_image", 4096, NULL, 10, NULL, 0);
}
