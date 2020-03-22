#ifndef HAL_KEY_H__
#define HAL_KEY_H__

#include "osCore.h"
struct Key_t
{
  uint8 key;
  uint8 up;
  uint8 down;
  uint8 keep;
  uint8 plong;
  uint8 pllong;
  uint32 runtime;
};

#define KEY0	0x01
#define KEY1	0x02
#define KEY2	0x04
#define KEY3	0x08

#define HAL_KEY_DOWN      0x01  // Joystick down
#define HAL_KEY_KEEP      0x02  // Joystick keep
#define HAL_KEY_LONG      0x03  // Joystick long press
#define HAL_KEY_LLONG     0x04  // Joystick long press
#define HAL_KEY_UP        0x05  // Joystick up


void hal_key_init(void);
uint8 hal_get_key(uint8 *val);
int hal_key_keeptime(void);

#endif
