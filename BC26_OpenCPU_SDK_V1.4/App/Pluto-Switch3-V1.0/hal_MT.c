/*
 * hal_MT.c
 *
 *  Created on: 2019Äê5ÔÂ23ÈÕ
 *      Author: zigbee
 */


/*
 * hal_uart.c
 *
 *  Created on: 2016éªžï¿½5éˆï¿½9éƒï¿½
 *      Author: lort
 */

#include "osCore.h"
#include "hal_MT.h"
#include "hal_uart.h"

#define HAL_UART_SOF_FLAG				0xFE
#define HAL_UART_SOF_FLAG1				0xFF
#define HAL_UART_RX_BUFFER_SIZE			(1024)
#define HAL_UART_MIN_PACKAGE_SIZE		(6)

typedef struct
{
	uint8 SOF;
	uint8 LEN[2];
	uint8 CMD0;
	uint8 CMD1;
	uint8 DATA[0];
}uart_package_t;

typedef struct
{
	uint16 	timeout;
	uint16 	cnt;
	uint8	data[HAL_UART_RX_BUFFER_SIZE];
}uart_rx_t;

static hal_uart_receive_cb_t	uart_recieve_cb = NULL;
static uart_rx_t				uart_rx;
static uint8					uart_rx_lock = osFalse;

static int 						uart_write(uint8 *pbuf, int len);
static int 						find_head(uint8 *s, uint16 len, uint16 index);
static int 						check_package(uart_package_t *pkg, int len);
static int 						read_package(uint8 *pbuf, int len);

static uart_package_t 			*package(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);
static int 						package_length(uart_package_t *pkg);
static int 						package_data_length(uart_package_t *pkg);
static uint8 					get_xor(uint8 *pbuf, int len);

osState hal_mt_init(hal_uart_receive_cb_t cb)
{
	osLogI(DBG_MT_UART,"hal_mt_init\r\n");
	uart_recieve_cb = cb;
	return osSucceed;
}

void hal_mt_input_byte(char byte)
{
	int rlen;
	if(HAL_UART_RX_BUFFER_SIZE>uart_rx.cnt)
	{
		uart_rx.data[uart_rx.cnt] = byte;
		uart_rx.cnt += 1;
	}
	else
	{
		uart_rx.cnt = 0;
	}
    uart_rx.timeout = 3;
}
void hal_mt_input_bytes(uint8 *pdata, int len)
{
	int rlen;
	if(HAL_UART_RX_BUFFER_SIZE>uart_rx.cnt)
	{
		osMemcpy(&uart_rx.data[uart_rx.cnt],pdata,len);
		uart_rx.cnt += len;
	}
	else
	{
		uart_rx.cnt = 0;
	}
    uart_rx.timeout = 3;
}
void  hal_mt_poll(void)
{
	uart_package_t 	*pkg;
	int	slen,dlen;
	pkg = (uart_package_t*)osMalloc(1024);
	if(pkg!=NULL)
	{
		if(uart_rx_lock==osFalse)
		{
			osLogI(0,"zb_uart_task:cmd0: try to read package now\r\n");
			if((slen = read_package((uint8*)pkg,1024))>0)
			{
				dlen = package_data_length(pkg);
				osLogI(DBG_MT_UART,"zb_uart_thread read package :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
				osLogB(DBG_MT_UART,"data:",pkg->DATA,slen);
				if(uart_recieve_cb!=NULL)
				{
					uart_recieve_cb(pkg->CMD0,pkg->CMD1,pkg->DATA,dlen);
				}
			}
		}
		osFree(pkg);
	}
}
osState  hal_mt_write(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	uart_package_t 	*pkg;
	uint8			ret = osFailed;
	int				slen;
	osLogI(DBG_MT_UART,"zb_uart_write:cmd0:%02x cmd1:%02x ",cmd0,cmd1);
	osLogB(DBG_MT_UART,"data:",pdata,len);
	pkg = package(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = package_length(pkg);
		osLogB(DBG_MT_UART,"zb_uart_write:",(uint8*)pkg,slen);
		if((uart_write((uint8 *)pkg,slen))==0)
		{
			ret = osSucceed;
		}
		osFree(pkg);
	}
	return ret;
}
int  hal_mt_read(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	uart_package_t 		*pkg;
	int					slen = 0, dlen,temp;
	uint8 				stry = 60;
	osLogI(DBG_MT_UART,"zb_uart_read at :cmd0:%02x cmd1:%02x \r\n",cmd0,cmd1);
	slen = len+HAL_UART_MIN_PACKAGE_SIZE;
	pkg = (uart_package_t*)osMalloc(slen);
	if(pkg!=NULL)
	{
		uart_rx_lock = osTrue;
		while(stry>0)
		{
			if((slen = read_package((uint8*)pkg,slen))>0)
			{
				if((pkg->CMD0==cmd0)&&(pkg->CMD1==cmd1))
				{
					dlen = package_data_length(pkg);
					if(dlen>len)
						dlen = len;
					osMemcpy(pdata,pkg->DATA,dlen);
					break;
				}
				else
				{
					if(uart_recieve_cb!=NULL)
					{
						dlen = package_data_length(pkg);
						uart_recieve_cb(pkg->CMD0,pkg->CMD1,pkg->DATA,dlen);
					}
				}
			}
			else
			{
				osSleep(10);
			}
			stry--;
		}
		uart_rx_lock = osFalse;
		osFree(pkg);
	}
	return slen;
}
static int  uart_write(uint8 *pbuf, int len)
{
	hal_uart_send(pbuf,len);
	return 0;
}
static int  find_head(uint8 *s, uint16 len, uint16 index)
{
	uint16 i;
	uint16 cnt=0;
	for(i=0;i<len;i++)
	{
		if((s[i]==HAL_UART_SOF_FLAG1)||(s[i]==HAL_UART_SOF_FLAG))
		{
			if(cnt>=index)
			{
				return i;
			}
			cnt++;
		}
	}
	return -1;
}
static int  read_package(uint8 *pbuf, int len)
{
	int 				headID = -1;
	int					slen;
	uint8				itry;
	//osEntercritical();
	if(uart_rx.timeout)
	{
		uart_rx.timeout--;
	}
	else if(uart_rx.cnt)
	{
		osLogB(DBG_MT_UART, "message timeout:",uart_rx.data,uart_rx.cnt);
		uart_rx.cnt = 0;
	}
	slen = 0;
	if(uart_rx.cnt>0)
	{
		itry=3;
		while(itry--)
		{
			headID = find_head(uart_rx.data,uart_rx.cnt,0);//find first head index
			if(headID>0)
			{
				uart_rx.cnt -= headID;
				osMemcpy(uart_rx.data,&uart_rx.data[headID],uart_rx.cnt);
			}
			else
				break;
		}
		if(headID==0)
		{
			if((slen = check_package((void*)uart_rx.data,uart_rx.cnt))>0)
			{
				osLogB(DBG_MT_UART,"read_package:",uart_rx.data,slen);
				slen = (slen>len)?len:slen;
				osMemcpy(pbuf,uart_rx.data,slen);
				uart_rx.cnt-=slen;
				osMemcpy(uart_rx.data,&uart_rx.data[slen],uart_rx.cnt);
				osLogI(DBG_MT_UART,"read_package check_package OK: slen:%d len:%d\r\n",slen,len);
			}
		}
	}
	//osExitcritical();
	return slen;
}
static int  check_package(uart_package_t *pkg, int len)
{
	int slen = 0,dlen;
	uint8 sum;
	osLogB(0,"check_package: ",(void*)pkg,len);
	if(len>=HAL_UART_MIN_PACKAGE_SIZE)
	{
		slen = package_length(pkg);
		if(slen<=len)
		{
			osLogI(DBG_MT_UART,"check_package: slen:%d len:%d \r\n",slen,len);
			if(pkg->SOF==HAL_UART_SOF_FLAG)
			{
				dlen = package_data_length(pkg);
				sum = get_xor(&pkg->LEN[0],(slen-2));
				if(sum==pkg->DATA[dlen])
					return (int)slen;
				else
				{
					osLogI(DBG_MT_UART,"check pacakge: sum error msg sum:%02x cal:%02x \r\n",pkg->DATA[dlen],sum);
				}
			}
			else if(pkg->SOF==HAL_UART_SOF_FLAG1)
			{
				return (int)slen;
			}
		}

	}
	return 0;
}

static uart_package_t  *package(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	uart_package_t 	*pkg;
	int 	slen;
	slen = len + HAL_UART_MIN_PACKAGE_SIZE;
	pkg = (uart_package_t*)osMalloc(slen);
	if(pkg!=NULL)
	{
		pkg->SOF = HAL_UART_SOF_FLAG;
		pkg->CMD0 = cmd0;
		pkg->CMD1 = cmd1;
		pkg->LEN[0] = (uint8)(len>>8);
		pkg->LEN[1] = (uint8)len;
		if((pdata!=NULL)&&(len!=0))
			osMemcpy(pkg->DATA,pdata,len);
		pkg->DATA[len] = get_xor(&pkg->LEN[0],(slen-2));
		return pkg;
	}
	return NULL;
}
static int  package_length(uart_package_t *pkg)
{
	int 	len;
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	len = temp+HAL_UART_MIN_PACKAGE_SIZE;
	return len;
}
static int  package_data_length(uart_package_t *pkg)
{
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	return (int)temp;
}
static uint8  get_xor(uint8 *pbuf, int len)
{
	int i;
	uint8 sum;
	sum = 0;
	for(i=0;i<len;i++)
	{
		sum ^= pbuf[i];
	}
	return sum;
}



