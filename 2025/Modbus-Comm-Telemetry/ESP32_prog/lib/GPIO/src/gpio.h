#pragma once

/* INCLUDES */
#include <driver/gpio.h>

/* DEFINES */
#define GPIO_HIGH   1
#define GPIO_LOW    0

#define USR_LED     GPIO_NUM_2

/* FUNCTIONS */
void setup_pins(void);
