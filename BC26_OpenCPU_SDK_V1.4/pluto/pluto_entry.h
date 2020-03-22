/*
 * pluto_service.h
 *
 *  Created on: 2016Äê8ÔÂ31ÈÕ
 *      Author: lort
 */

#ifndef PLUTO_PLUTO_ENTRY_H_
#define PLUTO_PLUTO_ENTRY_H_

typedef void  (*led_blink_cb_t)(int num,int htime, int ltime);

void pluto_init(void);
void pluto_registe_led(led_blink_cb_t cb);
void pluto_led_blink(int num, int htime, int ltime);

#endif /* PLUTO_PLUTO_ENTRY_H_ */
