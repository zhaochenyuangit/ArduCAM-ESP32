#include "driver/spi_master.h"
#include "driver/gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SPI_PORT HSPI_HOST
#define PIN_NUM_CLK GPIO_NUM_18
#define PIN_NUM_MISO GPIO_NUM_26
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CS GPIO_NUM_5

#define SPI_MAX_TRANS_SIZE 9600 //multiple times of 32 (for DMA) and 6 (for base64 encoder)

esp_err_t spi_master_init(void);

uint8_t spi_transfer_reg(uint8_t address, uint8_t send_value);
uint8_t spi_transfer_8(uint8_t send_value);
void spi_transfer_bytes(uint8_t command, uint8_t *send_buf, uint8_t *receive_buf, size_t size);

esp_err_t spi_read_reg(uint8_t address, uint8_t *receive_value);
esp_err_t spi_write_reg(uint8_t address, uint8_t send_value);

#ifdef __cplusplus
}
#endif