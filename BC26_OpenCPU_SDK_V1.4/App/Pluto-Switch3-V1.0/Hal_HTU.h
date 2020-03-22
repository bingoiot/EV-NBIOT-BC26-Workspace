
#ifndef __HAL_HTU_H__
#define __HAL_HTU_H__

#include "hal_i2c.h"

#define HTU21DADDR 0x80		//SHT21µÄIICµØÖ·

void HTU21DInit(void);
int HTU21DWork(unsigned char);

#endif

