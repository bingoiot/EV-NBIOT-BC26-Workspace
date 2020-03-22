
#include "osType.h"
#include "hal_key.h"

#define DBG_HAL_KEY		1

struct Key_t Button;

static void  hal_key_scan(void)
{
	uint8 temp;
	temp=0x00;
	if(!(Ql_GPIO_GetLevel(PINNAME_GPIO2)))
		temp|=KEY0;
	//if(!(Ql_GPIO_GetLevel(PINNAME_GPIO4)))
	//	temp|=KEY1;
	//if(!(Ql_GPIO_GetLevel(PINNAME_GPIO5)))
	//	temp|=KEY2;
	if(temp)
	{
		//DBG_vPrintf(TRUE,"key event :%d \n",temp);
		if(Button.keep==0x00)
		{
			Button.keep=0x01;
			Button.down=0x01;
			Button.key=temp;
			Button.runtime=0;
			Button.up=0x00;
		}
		else
		{
			Button.runtime+=20;
		}
	}
	else
	{
		if(Button.keep)
		{
			Button.keep=0x00;
			Button.up=0x01;
			Button.runtime=0;
		}
	}
}
void  hal_key_init(void)
{
	osPrintf(DBG_HAL_KEY,"hal_led_init: \r\n");
	Ql_GPIO_Init(PINNAME_GPIO2,//Enum_PinName       pinName,
				PINDIRECTION_IN,//Enum_PinDirection  dir,
				PINLEVEL_HIGH,//Enum_PinLevel      level,
				PINPULLSEL_PULLUP//Enum_PinPullSel    pullSel
	            );
	/*Ql_GPIO_Init(PINNAME_GPIO4,//Enum_PinName       pinName,
				PINDIRECTION_IN,//Enum_PinDirection  dir,
				PINLEVEL_HIGH,//Enum_PinLevel      level,
				PINPULLSEL_PULLUP//Enum_PinPullSel    pullSel
	            );
	Ql_GPIO_Init(PINNAME_GPIO5,//Enum_PinName       pinName,
				PINDIRECTION_IN,//Enum_PinDirection  dir,
				PINLEVEL_HIGH,//Enum_PinLevel      level,
				PINPULLSEL_PULLUP//Enum_PinPullSel    pullSel
	            );*/
}
int  hal_key_keeptime(void)
{
  return Button.runtime;
}
uint8  hal_get_key(uint8 *val)
{
  hal_key_scan();
  *val=Button.key;
  if(Button.plong)
  {
	Button.plong=0;
	return  HAL_KEY_LONG;
  }
  else if(Button.up)
  {
	  Button.up=0;
	  return HAL_KEY_UP;
  }
  else if(Button.down)
  {
      Button.down=0;
      return HAL_KEY_DOWN;
  }
  else if(Button.keep)
  {
    return HAL_KEY_KEEP;
  }
  *val=0x00;
  return 0x00;
}
