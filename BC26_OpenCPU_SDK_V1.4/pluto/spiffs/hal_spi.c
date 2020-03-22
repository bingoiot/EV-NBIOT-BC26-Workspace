/*
 * hal_spi.c
 *
 *  Created on: 2019Äê3ÔÂ29ÈÕ
 *      Author: zigbee
 */

#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_gpio.h"
#include "ql_spi.h"
#include "ql_error.h"
#include "hal_spi.h"
#include "osCore.h"

#define DBG_SPI_FLASH	1

//SPI_TYPE = 0 is analog spi, SPI_TYPE = 1 is hardware spi.
#define SPI_TYPE 					(0)
/***************************************************************************
* This example is used for test SPI funtion by hardware.
* And the peripheral SPI flash type is MX25L1606E
****************************************************************************/
#define USR_SPI_CHANNAL		     	(1)

int hal_spi_init(void)
{
	int ret = 0;
	osLogI(DBG_SPI_FLASH,"hal_spi_init.\r\n");
	ret = Ql_SPI_Init(USR_SPI_CHANNAL,PINNAME_SPI_SCLK,PINNAME_SPI_MISO,PINNAME_SPI_MOSI,PINNAME_SPI_CS,SPI_TYPE);
	osLogI(DBG_SPI_FLASH,"hal_spi_init :%d\r\n",ret);
	ret = Ql_SPI_Config(USR_SPI_CHANNAL,1,0,0,(2000)); //config sclk about 40MHz;
	osLogI(DBG_SPI_FLASH,"hal_spi_init :%d\r\n",ret);
    if (!SPI_TYPE)
    {
    	Ql_GPIO_Init(PINNAME_SPI_CS,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);	 //CS high
    }
	return ret;
}

void hal_spi_cs(uint8 cs)
{
	if (!SPI_TYPE)
	{
		if (cs)
			Ql_GPIO_SetLevel(PINNAME_SPI_CS,PINLEVEL_HIGH);
		else
			Ql_GPIO_SetLevel(PINNAME_SPI_CS,PINLEVEL_LOW);
	}
}

