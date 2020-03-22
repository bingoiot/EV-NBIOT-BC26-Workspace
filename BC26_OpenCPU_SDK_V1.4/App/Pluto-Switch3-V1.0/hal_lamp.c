/*
 * hal_lamp.c
 *
 *  Created on: 2016Äê11ÔÂ10ÈÕ
 *      Author: Administrator
 */

#include "osType.h"
#include "hal_lamp.h"

#define DBG_HAL_LAMP	1

#define set_lamp0()		Ql_GPIO_SetLevel(PINNAME_RTS_AUX,LAMP_ON_LEVEL) //gpio_output_set(BIT12, 0, BIT12, 0)
#define reset_lamp0()	Ql_GPIO_SetLevel(PINNAME_RTS_AUX,LAMP_OFF_LEVEL)
#define get_lamp0()		Ql_GPIO_GetLevel(PINNAME_RTS_AUX)

#define set_lamp1()		Ql_GPIO_SetLevel(PINNAME_GPIO3,LAMP_ON_LEVEL)
#define reset_lamp1()	Ql_GPIO_SetLevel(PINNAME_GPIO3,LAMP_OFF_LEVEL)//gpio_output_set(0, BIT13, BIT13, 0)
#define get_lamp1()		Ql_GPIO_GetLevel(PINNAME_GPIO3)

#define set_lamp2()		Ql_GPIO_SetLevel(PINNAME_GPIO1,LAMP_ON_LEVEL)
#define reset_lamp2()	Ql_GPIO_SetLevel(PINNAME_GPIO1,LAMP_OFF_LEVEL)
#define get_lamp2()		Ql_GPIO_GetLevel(PINNAME_GPIO1)

void  hal_lamp_init(void)
{
	Ql_GPIO_Init(PINNAME_RTS_AUX,//Enum_PinName       pinName,
				PINDIRECTION_OUT,//Enum_PinDirection  dir,
				PINLEVEL_LOW,//Enum_PinLevel      level,
				PINPULLSEL_DISABLE//Enum_PinPullSel    pullSel
	            );
	Ql_GPIO_Init(PINNAME_GPIO3,//Enum_PinName       pinName,
				PINDIRECTION_OUT,//Enum_PinDirection  dir,
				PINLEVEL_LOW,//Enum_PinLevel      level,
				PINPULLSEL_DISABLE//Enum_PinPullSel    pullSel
	            );
	Ql_GPIO_Init(PINNAME_GPIO1,//Enum_PinName       pinName,
				PINDIRECTION_OUT,//Enum_PinDirection  dir,
				PINLEVEL_LOW,//Enum_PinLevel      level,
				PINPULLSEL_DISABLE//Enum_PinPullSel    pullSel
	            );
	hal_lamp_set(LAMP0|LAMP1|LAMP2|LAMP3,LAMP_OFF);
}

void  hal_lamp_set(uint8 lamps, uint8 state)
{
	osPrintf(DBG_HAL_LAMP,"hal_lamp_set:lamps:%02x val:%d \r\n",lamps,state);
  if(lamps&LAMP0)
  {
    if(state==LAMP_ON_LEVEL)
    	set_lamp0();
    else
    	reset_lamp0();
  }
  if(lamps&LAMP1)
  {
    if(state==LAMP_ON_LEVEL)
    	set_lamp1();
    else
    	reset_lamp1();
  }
  if(lamps&LAMP2)
  {
    if(state==LAMP_ON_LEVEL)
    	set_lamp2();
    else
    	reset_lamp2();
  }
}
void  hal_lamp_tolgle(uint8 lamps)
{
osPrintf(DBG_HAL_LAMP,"hal_lamp_tolgle:lamps:%02x \r\n",lamps);
  if(lamps&LAMP0)
  {
    if(get_lamp0()==LAMP_OFF_LEVEL)
    	set_lamp0();
    else
    	reset_lamp0();
  }
  if(lamps&LAMP1)
  {
	  if(get_lamp1()==LAMP_OFF_LEVEL)
		set_lamp1();
	else
		reset_lamp1();
  }
  if(lamps&LAMP2)
  {
	  if(get_lamp2()==LAMP_OFF_LEVEL)
		set_lamp2();
	else
		reset_lamp2();
  }
}
uint8  hal_lamp_get(uint8 lamps)
{
	osPrintf(DBG_HAL_LAMP,"hal_lamp_get:lamps:%02x \r\n",lamps);
  if(lamps&LAMP0)
  {
	if(get_lamp0()==LAMP_ON_LEVEL)
    {
      return LAMP_ON_LEVEL;
    }
    else
    {
       return LAMP_OFF_LEVEL;
    }
  }
  else if(lamps&LAMP1)
  {
	if(get_lamp1()==LAMP_ON_LEVEL)
    {
      return LAMP_ON_LEVEL;
    }
    else
    {
       return LAMP_OFF_LEVEL;
    }
  }
  else if(lamps&LAMP2)
  {
	if(get_lamp2()==LAMP_ON_LEVEL)
    {
      return LAMP_ON_LEVEL;
    }
    else
    {
       return LAMP_OFF_LEVEL;
    }
  }
  return LAMP_OFF_LEVEL;
}
