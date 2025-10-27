#include <driver/uart.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "esp_intr_alloc.h"
#include "soc/interrupts.h"

#include "modbus_rtu.h"
#include "circ_buffer.h"

#define MAX_FRAME_SIZE      11

#define MODBUS_UART_NUM      UART_NUM_2
#define MODBUS_TX_PIN        GPIO_NUM_17
#define MODBUS_RX_PIN        GPIO_NUM_16
#define MODBUS_BAUD_RATE     115200

#define uC_A 0x01
#define uC_B 0x02
#define uC_C 0x03
#define uC_D 0x04

TaskHandle_t hMODBUS_TX_Task, hMODBUS_RX_Task;
QueueHandle_t qMODBUS_RX_Task, qMODBUS_Frame_Task, qUART;
SemaphoreHandle_t sMODBUS_TX;

typedef struct {
    uint8_t id;
    uint8_t func;
} last_mb_t;

typedef struct {
    uint8_t frame[MAX_FRAME_SIZE];
    uint8_t length;
} complete_frame_t;

typedef enum {
    NO_DATA = 0,
    READ_FUNCTION,
    READ_LENGTH,
    READ_SUFFIX
} mb_state_t;

modbus_data_t current_modbus_data = {
    .register_value = 0,
    .device_id = 0,
    .new_data_available = false
};

static uint16_t mb_crc16(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    return crc;
}

static void mb_read_hold_regs(uint8_t slave_id, uint16_t start_addr, uint16_t reg_count) {
    uint8_t frame[8];
    frame[0] = slave_id;
    frame[1] = 0x03;
    frame[2] = start_addr >> 8;
    frame[3] = start_addr & 0xFF;
    frame[4] = reg_count >> 8;
    frame[5] = reg_count & 0xFF;
    uint16_t crc = mb_crc16(frame, 6);
    frame[6] = crc >> 8;
    frame[7] = crc & 0xFF;

    ESP_LOGI("MB", "Sending Frame...: %02X %02X %02X %02X %02X %02X %02X %02X", 
        frame[0], frame[1], frame[2], frame[3], frame[4], frame[5], frame[6], frame[7]);

    uart_write_bytes(MODBUS_UART_NUM, frame, sizeof(frame));
}

void MODBUS_Frame_Task(void*) {
    complete_frame_t frame;
    while(1)
    {
        if (xQueueReceive(qMODBUS_Frame_Task, &frame, portMAX_DELAY)) {
            ESP_LOGI("FRAME", "POSITIVE");
            current_modbus_data.device_id = frame.frame[0];
            current_modbus_data.register_value = (frame.frame[3] << 8) | frame.frame[4];
            current_modbus_data.new_data_available = true;
            xSemaphoreGive(sMODBUS_TX);
        }
    }
}

void MODBUS_RX_Task(void*) {
    uint8_t byte;
    uint8_t index = 0;
    uint8_t data_length = 0;
    uint8_t expected_length = 0;
    mb_state_t state = NO_DATA;
    uint8_t frame[MAX_FRAME_SIZE];
    complete_frame_t complete_frame;
    TickType_t lastByteTime = xTaskGetTickCount();
    while (1) {
        if (xQueueReceive(qMODBUS_RX_Task, &byte, pdMS_TO_TICKS(200))) {
            lastByteTime = xTaskGetTickCount();
            switch (state) {
                case NO_DATA:
                    if (byte == uC_A || byte == uC_B || byte == uC_C || byte == uC_D) {
                        frame[index++] = byte;
                        state = READ_FUNCTION;
                    }
                    break;
                case READ_FUNCTION:
                    if (index >= MAX_FRAME_SIZE) {
                        state = NO_DATA;
                        index = 0;
                        break;
                    }
                    frame[index++] = byte;
                    state = READ_LENGTH;
                    break;
                case READ_LENGTH:
                    if (index >= MAX_FRAME_SIZE) {
                        state = NO_DATA;
                        index = 0;
                        break;
                    }
                    frame[index++] = byte;
                    data_length = byte;
                    expected_length = 3 + data_length + 2;
                    
                    if (expected_length > MAX_FRAME_SIZE) {
                        state = NO_DATA;
                        index = 0;
                    } else {
                        state = READ_SUFFIX;
                    }
                    break;
                case READ_SUFFIX:
                    if (index >= MAX_FRAME_SIZE) {
                        state = NO_DATA; 
                        index = 0;
                        break;
                    }
                    frame[index++] = byte;
                    
                    if (index >= expected_length) {
                        uint16_t received_crc = (frame[index-2] << 8) | frame[index-1];
                        uint16_t calculated_crc = mb_crc16(frame, index - 2);

                        ESP_LOGI("MB", "Received Frame...: %02X %02X %02X %02X %02X %02X %02X %02X", 
                            frame[0], frame[1], frame[2], frame[3], frame[4], frame[5], frame[6], frame[7]);
                        
                        if (received_crc == calculated_crc) {
                            memcpy(complete_frame.frame, frame, index);
                            complete_frame.length = index;
                            xQueueSend(qMODBUS_Frame_Task, &complete_frame, 0);
                        } else {
                            ESP_LOGE("FRAME", "CRC ERROR!");
                            xSemaphoreGive(sMODBUS_TX);
                        }
                        index = 0;
                        data_length = 0;
                        expected_length = 0;
                        state = NO_DATA;
                    }
                    break;
            }
        } else {
            if (xTaskGetTickCount() - lastByteTime >= pdMS_TO_TICKS(5000)) {
                lastByteTime = xTaskGetTickCount();
                ESP_LOGW("RX", "Frame timeout, resetting parser");
                xSemaphoreGive(sMODBUS_TX);
                state = NO_DATA;
                index = 0;
                data_length = 0;
                expected_length = 0;
            }
        }
    }
    vTaskDelete(NULL);
}

void MODBUS_TX_Task(void*) {
    uint8_t val = 0;
    while(1) 
    {
        if (xSemaphoreTake(sMODBUS_TX, portMAX_DELAY) == pdTRUE) {
            vTaskDelay(pdMS_TO_TICKS(3000));
            
            switch(val) {
                case 0:
                    mb_read_hold_regs(0x01, 0x0001, 0x0001);
                    val = 1;
                    break;
                case 1:
                    mb_read_hold_regs(0x01, 0x0002, 0x0001);
                    val = 0;
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

static void UART_Event_Task(void *pvParameters) {
    uart_event_t event;
    uint8_t buffer[128];
    for (;;) {
        if (xQueueReceive(qUART, &event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA: {
                    int len = uart_read_bytes(MODBUS_UART_NUM, buffer, event.size, portMAX_DELAY);
                    if (len > 0) {
                        for (int i = 0; i < len; i++) {
                            xQueueSend(qMODBUS_RX_Task, &buffer[i], 0);
                        }
                    }
                    break;
                }
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    ESP_LOGW("UART", "Buffer overflow — flushing RX");
                    uart_flush_input(MODBUS_UART_NUM);
                    xQueueReset(qUART);
                    break;
                default:
                    break;
            }
        }
    }
}

void mb_init(void) {
    qUART = xQueueCreate(10, sizeof(uart_event_t));
    qMODBUS_RX_Task = xQueueCreate(100, sizeof(uint8_t));
    qMODBUS_Frame_Task = xQueueCreate(10, sizeof(complete_frame_t));
    sMODBUS_TX = xSemaphoreCreateBinary();
    xSemaphoreGive(sMODBUS_TX);

    uart_config_t uart_config = {
        .baud_rate = MODBUS_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    esp_err_t ret;
    ret = uart_param_config(MODBUS_UART_NUM, &uart_config);
    if (ret != ESP_OK) ESP_LOGE("UART", "uart_param_config() Error: %d", ret);
    ret = uart_set_pin(MODBUS_UART_NUM, MODBUS_TX_PIN, MODBUS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) ESP_LOGE("UART", "uart_set_pin() Error: %d", ret);
    ret = uart_driver_install(MODBUS_UART_NUM, 1024, 1024, 20, &qUART, 0);
    if (ret != ESP_OK) ESP_LOGE("UART", "uart_driver_install() Error: %d", ret);


    xTaskCreate(MODBUS_RX_Task, "MODBUS_RX", 4096, NULL, 2, &hMODBUS_RX_Task);
    xTaskCreate(MODBUS_TX_Task, "MODBUS_TX", 4096, NULL, 1, &hMODBUS_TX_Task);
    xTaskCreate(MODBUS_Frame_Task, "MODBUS_Frame", 4096, NULL, 2, NULL);
    xTaskCreate(UART_Event_Task, "UART_Event", 4096, NULL, 3, NULL);
}
