#pragma once

#include <stdbool.h>

typedef struct {
    uint16_t register_value;
    uint8_t device_id;
    bool new_data_available;
} modbus_data_t;

extern TaskHandle_t hMODBUS_TX_Task, hMODBUS_RX_Task;
extern QueueHandle_t qMODBUS_TX_Task, qMODBUS_RX_Task;
extern modbus_data_t current_modbus_data;


void mb_init(void);
void MODBUS_TX_Task(void*);