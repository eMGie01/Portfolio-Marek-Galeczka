#include <freertos/FreeRTOS.h>
#include "gpio.h"

void setup_pins(void) {
    // output pins: SHDN_SYS / PWR_LED / SHDN_24
    gpio_config_t io_conf_output = {
        .pin_bit_mask = (1ULL << USR_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_output);
    // init levels
    gpio_set_level(USR_LED, GPIO_LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}