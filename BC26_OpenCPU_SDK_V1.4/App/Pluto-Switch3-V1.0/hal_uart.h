/*
 * hal_uart.h
 *
 *  Created on: 2019Äê5ÔÂ23ÈÕ
 *      Author: zigbee
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

void hal_uart_init(void);
void hal_uart_send(uint8 *p, int len);
int  hal_uart_read(uint8 *p, int len);

#endif /* BC26_OPENCPU_SDK_BETA0801_APP_GATEWAY_ZB_V01_HAL_UART_H_ */
