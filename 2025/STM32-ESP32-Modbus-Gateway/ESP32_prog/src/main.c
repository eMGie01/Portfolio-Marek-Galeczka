#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "modbus_rtu.h"
#include "gpio.h"
#include "wifi.h"

#define SHDN_TIME   1000    // ms

typedef enum {
    BUTTON_PRESSED,
    BUTTON_RELEASED
} pwr_sw_event_t;

void app_main()
{
    setup_pins();
    wifi_init_ap();
    mb_init();
}   