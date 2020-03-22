/*
 * Hal_LED.c
 *
 *  Created on: 2014-8-29
 *      Author: Administrator
 */

#include "osCore.h"
#include "hal_led.h"

#define DBG_HAL_LED		1
struct Hal_led_t Led;

void hal_led_task(int sig, void *param,int len);

void  hal_led_init(void)
{
	osPrintf(DBG_HAL_LED,"hal_led_init: \r\n");
	Ql_GPIO_Init(	PINNAME_NETLIGHT,//Enum_PinName       pinName,
						PINDIRECTION_OUT,//Enum_PinDirection  dir,
						PINLEVEL_LOW,//Enum_PinLevel      level,
						PINPULLSEL_DISABLE//Enum_PinPullSel    pullSel
	                 );
	hal_led_set(LED_STATUS_OFF);
}
void  hal_led_set(uint8 val)
{
	//osPrintf(DBG_HAL_LED,"hal_led_set: state:%d  \r\n",val);
	Led.lastStat = val;
	Ql_GPIO_SetLevel(PINNAME_NETLIGHT,val);
}
void  hal_led_blink(int blinkNum, int blinkTimeH, int blinkTimeL)
{
	osPrintf(DBG_HAL_LED,"hal_led_blink:num:%d h:%d l:%d \r\n",blinkNum,blinkTimeH,blinkTimeL);
	Led.status = LED_STATUS_OFF;
	hal_led_set(LED_STATUS_OFF);
	Led.blinkNum=blinkNum;
	if(blinkTimeH==0)
		blinkTimeH=DEFAULT_BLINK_TIME_H;
	if(blinkTimeL==0)
		blinkTimeL=DEFAULT_BLINK_TIME_L;
	Led.blinkTimeH=(blinkTimeH);
	Led.blinkTimeL=(blinkTimeL);
	if(Led.blinkNum)
	{
		if(Led.status==LED_STATUS_OFF)
			osStartTimer(Led.blinkTimeL,hal_led_task,1,NULL);
		else
			osStartTimer(Led.blinkTimeH,hal_led_task,1,NULL);
	}
	else
		osSendMessage(hal_led_task,1,NULL,0);
}
void  hal_led_task(int sig, void *param,int len)
{
	if(Led.blinkNum>0)
	{
		if(Led.status==LED_STATUS_OFF)
		{
			hal_led_set(LED_STATUS_ON);
			osStartTimer(Led.blinkTimeH,hal_led_task,1,NULL);
		}
		else
		{
			hal_led_set(LED_STATUS_OFF);
			Led.blinkNum--;
			osStartTimer(Led.blinkTimeL,hal_led_task,1,NULL);
		}
		Led.status = !Led.status;
	}
	else
		hal_led_set(Led.lastStat);
}
