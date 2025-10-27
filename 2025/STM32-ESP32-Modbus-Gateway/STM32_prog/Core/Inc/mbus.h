/*
 * mbus.h
 *
 *  Created on: Oct 23, 2025
 *      Author: galeczka
 */

#ifndef INC_MBUS_H_
#define INC_MBUS_H_


#define MB_REG_COUNT 2
#define REG_TEST 0

#include "circ_buffer.h"
#include "usart.h"


extern uint16_t holdingRegisters[MB_REG_COUNT];


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void mb_init(void);
bool mb_read(void);


#endif /* INC_MBUS_H_ */
