/*
 * hal_uart.c
 *
 *  Created on: 2019Äê5ÔÂ23ÈÕ
 *      Author: zigbee
 */

/*
 * jf_trace.c
 *
 *  Created on: 2016ï¿½ï¿½1ï¿½ï¿½6ï¿½ï¿½
 *      Author: Administrator
 */

#include "osType.h"
#include "osCore.h"
#include "stdarg.h"

#define HAL_UART_PORT		UART_PORT0

static void uart_int_handler(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);

void hal_uart_init(void)
{
	static uint8  init = 0;
	int ret;
	if(init==0)
	{
		ret = Ql_UART_Register(HAL_UART_PORT, uart_int_handler, NULL);
		if (ret < QL_RET_OK)
		{
			Ql_Debug_Trace("Fail to register serial port[%d], ret=%d\r\n", HAL_UART_PORT, ret);
		}
		ret = Ql_UART_Open(HAL_UART_PORT, 115200, FC_NONE);
		if (ret < QL_RET_OK)
		{
			Ql_Debug_Trace("Fail to open serial port[%d], ret=%d\r\n", HAL_UART_PORT, ret);
		}
		init = 1;
	}
}

void hal_uart_send(uint8 *p, int len)
{
	Ql_UART_Write((Enum_SerialPort)(HAL_UART_PORT), (u8*)(p), len);
}
int hal_uart_read(uint8 *p, int len)
{
	int slen;
	slen = Ql_UART_Read((Enum_SerialPort)HAL_UART_PORT, p, len);
	return slen;
}

static void uart_int_handler(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{

}








