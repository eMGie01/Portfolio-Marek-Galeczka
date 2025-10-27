#include <esp_http_server.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <string.h>

#include "modbus_rtu.h"
#include "wifi.h"
#include "gpio.h"

#define SSID    "ESP32-S2-mini"
#define PASS    "1234567890"

extern const char _binary_index_html_start[]    asm("_binary_index_html_start");
extern const char _binary_index_html_end[]      asm("_binary_index_html_end");
extern const char _binary_style_css_start[]     asm("_binary_style_css_start");
extern const char _binary_style_css_end[]       asm("_binary_style_css_end");
extern const char _binary_script_js_start[]     asm("_binary_script_js_start");
extern const char _binary_script_js_end[]       asm("_binary_script_js_end");

static httpd_handle_t server = NULL;
static bool led_state = false;

static esp_err_t h_modbus_rtu(httpd_req_t *req) {
    char response[100];

    snprintf(response, sizeof(response), "{\"device_id\":\"0x%02X\",\"value\":%u,\"hex\":\"0x%04X\"}", 
                current_modbus_data.device_id,
                current_modbus_data.register_value,
                current_modbus_data.register_value);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

static esp_err_t h_index_html(httpd_req_t *req) {
    ssize_t length = (ssize_t)_binary_index_html_end - (ssize_t)_binary_index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, _binary_index_html_start, length);
    return ESP_OK;
}

static esp_err_t h_style_css(httpd_req_t *req) {
    ssize_t length = (ssize_t)_binary_style_css_end - (ssize_t)_binary_style_css_start;
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, _binary_style_css_start, length);
    return ESP_OK;
}

static esp_err_t h_script_js(httpd_req_t *req) {
    ssize_t length = (ssize_t)_binary_script_js_end - (ssize_t)_binary_script_js_start;
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, _binary_script_js_start, length);
    return ESP_OK;
}

static esp_err_t h_toggle_led(httpd_req_t *req) {
    led_state = !led_state;
    gpio_set_level(USR_LED, led_state);
    
    char response[50];
    snprintf(response, sizeof(response), "{\"status\":\"ok\",\"led_state\":%s}", led_state ? "true" : "false");
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

static void wifi_start_webserver(void) {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = 80;
    
    if (httpd_start(&server, &cfg) == ESP_OK) {
        httpd_uri_t html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = h_index_html
        };
        httpd_register_uri_handler(server, &html);
        httpd_uri_t css = {
            .uri = "/style.css",
            .method = HTTP_GET, 
            .handler = h_style_css
        };
        httpd_register_uri_handler(server, &css);
        httpd_uri_t js = {
            .uri = "/script.js",
            .method = HTTP_GET,
            .handler = h_script_js
        };
        httpd_register_uri_handler(server, &js);
        httpd_uri_t toggle = {
            .uri = "/api/toggle",
            .method = HTTP_POST,
            .handler = h_toggle_led
        };
        httpd_register_uri_handler(server, &toggle);
        httpd_uri_t modbus = {
            .uri = "/api/modbus",
            .method = HTTP_GET,
            .handler = h_modbus_rtu
        };
        httpd_register_uri_handler(server, &modbus);
    }
}

void wifi_init_ap(void) {

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_ap();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_cfg = {
        .ap = {
            .ssid = SSID,
            .ssid_len = strlen(SSID),
            .channel = 1,
            .password = PASS,
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .beacon_interval = 100,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_start_webserver();
}