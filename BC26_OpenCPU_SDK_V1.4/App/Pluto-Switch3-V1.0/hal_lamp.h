/*
 * hal_lamp.h
 *
 *  Created on: 2016Äê11ÔÂ10ÈÕ
 *      Author: Administrator
 */

#ifndef HAL_DRIVER_HAL_LAMP_H_
#define HAL_DRIVER_HAL_LAMP_H_

#include "osCore.h"



#define LAMP_ON   0x01
#define LAMP_OFF  0x00

#define LAMP_ON_LEVEL	0x01
#define LAMP_OFF_LEVEL	0x00

#define LAMP0 0x01
#define LAMP1 0x02
#define LAMP2 0x04
#define LAMP3 0x08

void hal_lamp_init(void);
void hal_lamp_set(uint8 lamps, uint8 state);
void hal_lamp_tolgle(uint8 lamps);
uint8 hal_lamp_get(uint8 lamps);


#endif /* HAL_DRIVER_HAL_LAMP_H_ */
