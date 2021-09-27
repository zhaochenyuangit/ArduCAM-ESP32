#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

// I2C Defines
#define I2C_SCL_IO 22          // GPIO number for I2C master clock
#define I2C_SDA_IO 21          // GPIO number for I2C master data
#define I2C_FREQ_HZ 400000     // I2C master clock frequency
#define I2C_PORT_NUM I2C_NUM_1 // I2C port number for master dev, can be I2C_NUM_0 ~ (I2C_NUM_MAX-1)
#define I2C_TX_BUF_DISABLE 0   // No need for buffer
#define I2C_RX_BUF_DISABLE 0

// I2C Common Protocol Defines
#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1         // I2C master will check ACK from slave
#define ACK_CHECK_DIS 0x0        // I2C master will not check ACK from slave
#define ACK_VAL I2C_MASTER_ACK   // 0x0
#define NACK_VAL I2C_MASTER_NACK // 0x1

    esp_err_t i2c_master_init(void);

    esp_err_t i2c_master_read_slave(uint8_t dev_addr, uint8_t i2c_reg, uint8_t *data_rd, uint8_t size);

    esp_err_t i2c_master_write_slave(uint8_t dev_addr, uint8_t i2c_reg, uint8_t *data_wr, uint8_t size);

#ifdef __cplusplus
}
#endif