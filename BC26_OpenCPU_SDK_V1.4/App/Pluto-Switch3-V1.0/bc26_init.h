/*
 * bc26_ init.h
 *
 *  Created on: 2019Äê3ÔÂ20ÈÕ
 *      Author: zigbee
 */

#ifndef PLUTO_SOCKET_BC26_INIT_H_
#define PLUTO_SOCKET_BC26_INIT_H_

typedef enum
{
	Disable_eDRx = 0,
	Enable_eDRx = 1,
	Enable_eDRxUnsolicited = 2,
	Disable_eDRxAll = 3
}eDRxMode_t;

extern void 		bc26_init(void);
extern osState 		bc26_isok(void);
extern osState 		bc26_setDRxMode(eDRxMode_t mode, uint8 type, char *eDRx_Value);
extern int 			bc26_query_rssi(void);

#endif /* PLUTO_SOCKET_BC26_INIT_H_ */
