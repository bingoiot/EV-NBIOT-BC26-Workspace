/*
 * factory_settings.c
 *
 *  Created on: 2017Äê1ÔÂ12ÈÕ
 *      Author: zigbee
 */

#include "osCore.h"
#include "pluto.h"
#include "cJSON.h"
#include "aID.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_MT.h"

#define DBG_FACTORY	0

typedef enum
{
	factory_sig_operation = 1,
}app_task_e;

static void factory_task(int sig,void *arg,int len);
static void mt_command_receive_cb(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);
static void response_device_info(void);
static void response_service_info(void);
static void write_service_info(uint8 *pdata, int len);
void FUC_ATTR factory_init(void)
{
	osStartReloadTimer(20,factory_task,factory_sig_operation,NULL);
	hal_mt_init(mt_command_receive_cb);
}
#define DEBUG_BUF_SIZE		1024
static  uint8 				uart_buf[DEBUG_BUF_SIZE];
static void FUC_ATTR factory_task(int sig,void *arg,int len)
{
	int slen;
	switch(sig)
	{
		case factory_sig_operation:
		slen = hal_uart_read(uart_buf,DEBUG_BUF_SIZE);
		if(slen>0)
		{
			hal_mt_input_bytes(uart_buf,slen);
		}
		hal_mt_poll();
		break;
	}
}
static void FUC_ATTR mt_command_receive_cb(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	osLogI(DBG_FACTORY,"mt_command_receive_cb :cmd0:%02x cmd1:%02x ",cmd0,cmd1);
	osLogB(DBG_FACTORY,"data:",pdata,len);
	if((cmd0&MT_RPC_CMD_SREQ)==MT_RPC_CMD_SREQ)//only process request  command
	{
		if((cmd0&MT_RPC_SYS_SYS)==MT_RPC_SYS_SYS)//only process system request layer
		{
			switch(cmd1)
			{
			case MT_SYS_TEST:
				break;
			case MT_SYS_RESET:
				pluto_reset_system(2000);
				break;
			case MT_SYS_READ_INFO://request read device info
				response_device_info();
				break;
			case MT_SYS_READ_SN://request read device serial number info of remote service
				response_service_info();
				break;
			case MT_SYS_WRITE_SN://request write device serial numer inof of remote service
				write_service_info(pdata,len);
				break;
			}
		}
	}
	else
	{

	}
}
#define  DEVICE_DESCRIBE_FILE_NAME		"DeviceInfo.sys"
static void FUC_ATTR response_device_info(void)
{
	char 	*text;
	cJSON 	*root;
	char 	chipID[8];
	char 	buf[64];
	text = (char*)pluto_read_text_file(DEVICE_DESCRIBE_FILE_NAME);
	if(text!=NULL)
	{
		root = cJSON_Parse(text);
		osFree(text);
		if(root!=NULL)
		{
			osMemset(chipID,0,sizeof(chipID));
			osU32toB(&chipID[4],pluto_get_chip_id());
			osBtoHexStr(buf,0x00,chipID,8);
			cJSON_AddStringToObject(root,"chip_id",buf);
			text = cJSON_Print(root);
			cJSON_Delete(root);
			hal_mt_write(MT_RPC_CMD_SRSP|MT_RPC_SYS_SYS,MT_SYS_READ_INFO,(uint8*)text,osStrlen(text));
			osFree(text);
		}
	}
}
static void FUC_ATTR response_service_info(void)
{
	char *info;
	info = (char*)pluto_read_sn();
	if(info!=NULL)
	{
		hal_mt_write(MT_RPC_CMD_SRSP|MT_RPC_SYS_SYS,MT_SYS_READ_SN,(uint8*)info,osStrlen(info));
		osFree(info);
	}
}
static void FUC_ATTR write_service_info(uint8 *pdata, int len)
{
	char *str;
	str = osBtoStr(pdata,len);
	if(str!=NULL){
		pluto_write_sn(str);
		osFree(str);
		response_service_info();
	}
}
