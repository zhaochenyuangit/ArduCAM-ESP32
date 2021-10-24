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
#include "cJSON.h"

#include "interface_i2c.h"
#include "interface_spi.h"
#include "network_common.h"

#define BASE64_BUF_SIZE ((SPI_MAX_TRANS_SIZE / 3 + 2) * 4)
#define CS_PIN GPIO_NUM_5

static esp_mqtt_client_handle_t client = NULL;
static SemaphoreHandle_t sema_arduchip = NULL;
static SemaphoreHandle_t sema_read_to_arduchip = NULL;
static QueueHandle_t q_read_to_send = NULL;
static SemaphoreHandle_t sema_send = NULL;

ArduCAM_OV2640 myCAM(CS_PIN);

static int64_t eclipse_time_ms(bool startend)
{
    enum
    {
        START = 0,
        END,
    };
    static struct timeval tik, tok;
    if (startend == START)
    {
        gettimeofday(&tik, NULL);
        return 0;
    }
    else
    {
        gettimeofday(&tok, NULL);
        int64_t time_ms = (int64_t)(tok.tv_sec - tik.tv_sec) * 1000 + (tok.tv_usec - tik.tv_usec) / 1000;
        return time_ms;
    }
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

void read_frame_buffer(void *arg)
{
    uint8_t *buffer = (uint8_t *)arg;
    while (1)
    {
        xSemaphoreTake(sema_arduchip, portMAX_DELAY);
        size_t len = myCAM.read_fifo_length();
        if (len >= SPI_MAX_TRANS_SIZE)
        {
            printf("Over spi max size.\n");
            return;
        }
        else if (len == 0)
        {
            printf("Size is 0.\n");
            return;
        }

        myCAM.CS_LOW();
        spi_transfer_bytes(BURST_FIFO_READ, buffer, buffer, len);
        myCAM.CS_HIGH();
        xSemaphoreGive(sema_read_to_arduchip);
        xQueueSend(q_read_to_send, &len, 0);
        xSemaphoreTake(sema_send, portMAX_DELAY);
    }
    buffer = NULL;
}

void mqtt_send_image(void *arg)
{
    uint8_t *binary_buf = (uint8_t *)arg;
    static uint8_t base64_buf[BASE64_BUF_SIZE];
    static size_t binary_len;
    static size_t encode_len;
    static const char *topic = "ov2640/base64";
    while (1)
    {
        xQueueReceive(q_read_to_send, &binary_len, portMAX_DELAY);
        mbedtls_base64_encode(base64_buf, BASE64_BUF_SIZE, &encode_len, binary_buf, binary_len);
        //printf("encode len %d\n", encode_len);
        base64_buf[encode_len] = '\0';
        xSemaphoreGive(sema_send);
        mqtt_send(client, topic, (char *)base64_buf, 0);
    }
    binary_buf = NULL;
}

void take_image(void *_)
{
    uint8_t *buffer = (uint8_t *)heap_caps_malloc(SPI_MAX_TRANS_SIZE, MALLOC_CAP_DMA);
    assert(buffer);
    xTaskCreatePinnedToCore(mqtt_send_image, "send image task", 4096, (void *)buffer, 10, NULL, 0);
    xTaskCreatePinnedToCore(read_frame_buffer, "read buffer task", 4096, (void *)buffer, 5, NULL, 1);
    while (1)
    {
        xSemaphoreTake(sema_read_to_arduchip, portMAX_DELAY);
        //printf("start capture\n");
        myCAM.clear_fifo_flag();
        myCAM.start_capture();
        //eclipse_time_ms(false);
        while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
            vTaskDelay(pdMS_TO_TICKS(2));
        //printf("capture total_time used (in miliseconds): %lld\n", eclipse_time_ms(true));
        xSemaphoreGive(sema_arduchip);
    }
    free(buffer);
    buffer = NULL;
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(start_wifi());
    ESP_ERROR_CHECK(start_mqtt(&client, MYMQTT, NULL, NULL));
    ESP_ERROR_CHECK(spi_master_init());
    ESP_ERROR_CHECK(i2c_master_init());

    sema_arduchip = xSemaphoreCreateBinary();
    sema_read_to_arduchip = xSemaphoreCreateBinary();
    q_read_to_send = xQueueCreate(1, sizeof(size_t));
    sema_send = xSemaphoreCreateBinary();
    assert(sema_arduchip);
    assert(sema_read_to_arduchip);
    assert(q_read_to_send);
    assert(sema_send);
    xSemaphoreGive(sema_read_to_arduchip);

    OV2640_valid_check();
    //Change to JPEG capture mode and initialize the OV2640 module
    eclipse_time_ms(0);
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.set_JPEG_size(OV2640_320x240);
    myCAM.clear_fifo_flag();
    printf("configure takes %lld ms\n", eclipse_time_ms(1));
    xTaskCreatePinnedToCore(take_image, "take_image", 4096, NULL, 2, NULL, 1);
}
