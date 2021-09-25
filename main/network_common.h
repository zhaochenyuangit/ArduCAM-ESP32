#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_netif.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYSSID "Racoon&Masters"
#define MYPWD "gotomunich2019"
#define MYMQTT "mqtt://192.168.178.42"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MQTT_CONNECTED_BIT BIT0
#define MQTT_ERROR_BIT BIT1

struct mqtt_msg
{
    char topic[20];
    char msg[20];
};

/**
 * @brief start wifi, reconnect when disconnect
 * @return ESP_OK when success
 */
esp_err_t start_wifi(void);
/**
 * @brief start mqtt
 * @param client a pointer to client handle (pointer to pointer)
 * @param uri the uri of the brocker
 * @param username set to NULL if no username
 * @param password set to NULL if no password
 * @return ESP_OK on success
 */
esp_err_t start_mqtt(esp_mqtt_client_handle_t *client, const char *uri, const char *username, const char *password);
/**
 * @brief publish message with mqtt
 * @param client the cilent handle
 * @param topic a string
 * @param data a string
 * @param qos 0, 1 or 2
 */
void mqtt_send(esp_mqtt_client_handle_t client, const char *topic, const char *data, int qos);
void mqtt_listen(esp_mqtt_client_handle_t client, const char *topic, QueueHandle_t *msg_q);

#ifdef __cplusplus
}
#endif