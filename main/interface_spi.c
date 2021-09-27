#include "interface_spi.h"
#include "string.h"

static spi_device_handle_t spi_handle = NULL;

esp_err_t spi_master_init(void)
{
    esp_err_t err;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SPI_MAX_TRANS_SIZE,
    };
    err = spi_bus_initialize(SPI_PORT, &buscfg, 1);
    if (err != ESP_OK)
        return err;
    spi_device_interface_config_t devcfg = {
        .command_bits = 8,
        .queue_size = 1,
        .clock_speed_hz = SPI_MASTER_FREQ_8M / 8 * 7, //must be strictly slow than ArduChip SPI 8MHz
        .mode = 0,
        .spics_io_num = -1, //capatibility with the ArduCAM library: enable CS pin manually
    };
    err = spi_bus_add_device(SPI_PORT, &devcfg, &spi_handle);
    return err;
}

esp_err_t spi_read_reg(uint8_t address, uint8_t *receive_value)
{
    spi_transaction_t t = {
        .cmd = address,
        .flags = SPI_TRANS_USE_RXDATA,
        .tx_buffer = NULL,
        .length = 8,
        .rxlength = 8,
    };
    esp_err_t err = spi_device_polling_transmit(spi_handle, &t);
    if (err != ESP_OK)
        printf("spi_read_8 err %d: %s\n", err, esp_err_to_name(err));
    *receive_value = t.rx_data[0];
    return err;
}

esp_err_t spi_write_reg(uint8_t address, uint8_t send_value)
{
    esp_err_t err;
    err = spi_device_acquire_bus(spi_handle, portMAX_DELAY);
    if (err != ESP_OK)
        return err;

    spi_transaction_t t = {
        .cmd = address,
        .length = 8,
        .rxlength = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .rx_buffer = NULL,
        .tx_data = {send_value},
    };
    err = spi_device_polling_transmit(spi_handle, &t);
    spi_device_release_bus(spi_handle);
    return err;
}

uint8_t spi_transfer_reg(uint8_t address, uint8_t send_value)
{
    esp_err_t err = spi_device_acquire_bus(spi_handle, portMAX_DELAY);
    ESP_ERROR_CHECK(err);
    spi_transaction_t t = {
        .cmd = address,
        .length = 8,
        .rxlength = 8,
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
        .tx_data = {send_value},
    };
    err = spi_device_polling_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(err);
    spi_device_release_bus(spi_handle);
    return t.rx_data[0];
}

uint8_t spi_transfer_8(uint8_t send_value)
{
    esp_err_t err = spi_device_acquire_bus(spi_handle, portMAX_DELAY);
    ESP_ERROR_CHECK(err);
    spi_transaction_t t = {
        .length = 8,
        .rxlength = 8,
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA | SPI_TRANS_VARIABLE_CMD,
        .tx_data = {send_value},
    };
    spi_transaction_ext_t ext_t = {
        .base = t,
        .command_bits = 0,
    };
    err = spi_device_polling_transmit(spi_handle, (spi_transaction_t *)&ext_t);
    ESP_ERROR_CHECK(err);
    spi_device_release_bus(spi_handle);
    return t.rx_data[0];
}

void spi_transfer_bytes(uint8_t command, uint8_t *send_buf, uint8_t *receive_buf, size_t size)
{
    spi_transaction_t t = {
        .cmd = command,
        .length = size * 8,
        .rxlength = size * 8,
        .tx_buffer = send_buf,
        .rx_buffer = receive_buf,
    };
    esp_err_t err = spi_device_transmit(spi_handle, &t); //for large data use SPI interrupt instead of polling
    ESP_ERROR_CHECK(err);
}