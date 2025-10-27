/*
 * mbus.c
 *
 *  Created on: Oct 23, 2025
 *      Author: galeczka
 */


#include "mbus.h"
#include "gpio.h"
#include <string.h>


#define uC 0x01 // 0x04
#define MAX_BUFFER_SIZE 128

typedef enum {
	// Read functions
	READ_COILS = 0x01,
	READ_DISC_INPUTS = 0x02,
	READ_HOLDING_REGS = 0x03,
	READ_INPUT_REFS = 0x04,
	// Write functions
	WRITE_COILS = 0x05,
	WRITE_SINGLE_REG = 0x06,
	WRITE_MULT_COILS = 0x07,
	WRITE_MULT_REGS = 0x08,
	// Other
	READ_EXC_STATUS = 0x07,
	DIAGNOSTICS = 0x08,
	REPORT_SLAVE_ID = 0x11
} Func;

uint16_t holdingRegisters[MB_REG_COUNT] = {0};

circ_buffer_t cb;
uint8_t buffer[MAX_BUFFER_SIZE] = {0};

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


static void mb_led_hand (bool func) {
	static uint32_t lastSysTick = 0;
	if (func) {
		HAL_GPIO_WritePin(LED_STATUSD_GPIO_Port, LED_STATUSD_Pin, GPIO_PIN_SET);
		lastSysTick = HAL_GetTick();
	} else if (HAL_GetTick() - lastSysTick > 40) {
		HAL_GPIO_WritePin(LED_STATUSD_GPIO_Port, LED_STATUSD_Pin, GPIO_PIN_RESET);
	}
}


static bool mb_send_regs(uint16_t addr, uint16_t count) {
	// id // 0x03 // byte count // data // crc
	if (addr > MB_REG_COUNT) return false;
	uint16_t size = (addr + count > MB_REG_COUNT) ? (MB_REG_COUNT - addr) : count;

	uint8_t frame[5 + size];
	frame[0] = uC;
	frame[1] = READ_HOLDING_REGS;
	frame[2] = size * 2;

	for (uint16_t i = 0; i < size; ++i) {
		uint16_t reg = holdingRegisters[addr + i];
		frame[3 + i*2] = reg >> 8;
		frame[4 + i*2] = reg & 0xFF;
	}

	uint16_t length = 3 + size * 2;
	uint16_t crc = mb_crc16(frame, length);
	frame[length] = crc >> 8;
	frame[length + 1] = crc & 0xFF;

	mb_led_hand(true);
	HAL_UART_Transmit(&huart1, frame, (length + 2), HAL_MAX_DELAY);
	return true;
}

static bool mb_parse_frame(uint8_t *frame) {

	Func func = frame[1];
	switch (func) {

	case READ_COILS:
		break;
	case READ_DISC_INPUTS:
		break;
	case READ_HOLDING_REGS:
		uint16_t addr = (frame[2] << 8) | frame[3];
		uint16_t count = (frame[4] << 8) | frame[5];
		return mb_send_regs(addr - 1, count);
	case READ_INPUT_REFS:
		break;
	case WRITE_COILS:
		break;
	case WRITE_SINGLE_REG:
		break;
	case WRITE_MULT_COILS:
		break;
	case WRITE_MULT_REGS:
		break;
	case REPORT_SLAVE_ID:
		break;
	}
	return false;
}


static void mb_reg_init(void) {
	holdingRegisters[0] = 0x1234;
	holdingRegisters[1] = 0x4321;
}


uint8_t uart_byte = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		if (!cb_buffer_push(&cb, (uint8_t*)&uart_byte, sizeof(uint8_t))) {
			HAL_GPIO_WritePin(LED_STATUSD_GPIO_Port, LED_STATUSD_Pin, GPIO_PIN_SET);
		}
		HAL_UART_Receive_IT(&huart1, &uart_byte, sizeof(uint8_t));
	}
}


void mb_init(void) {
	mb_reg_init();
	cb_buffer_init(&cb, buffer, MAX_BUFFER_SIZE);
	HAL_UART_Receive_IT(&huart1, &uart_byte, sizeof(uint8_t));
}


bool mb_read(void) {
	mb_led_hand(false);
	if (cb_taken_space(&cb) >= 8) {
		uint8_t frame[8];
		if (cb_buffer_pull(&cb, frame, 1) == 0) return false;

		if (frame[0] == uC) {
			if (cb_buffer_pull(&cb, frame + 1, 7) == 7) {
				uint16_t received_crc = (frame[6] << 8) | frame[7];
				uint16_t crc = mb_crc16(frame, 6);
				if (crc == received_crc)
					return mb_parse_frame(frame);
			} else {
				return false;
			}
		}
	}
	return false;
}











