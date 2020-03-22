/*
 * pluto_service.c
 *
 *  Created on: 2016Äê8ÔÂ31ÈÕ
 *      Author: lort
 */

#include "osCore.h"
#include "pluto.h"
#include "ril_network.h"
#include "pluto_entry.h"
#include "tcp_socket.h"
#include "udp_socket.h"

led_blink_cb_t blink_led;

const static nwk_adapter_t myadapter={
		.udp_open = udp_socket_init,
		.tcp_open = tcp_socket_init,
		.udp_close = udp_socket_deinit,
		.tcp_close = tcp_socket_deinit,
		.udp_output = udp_socket_send,
		.tcp_output = tcp_socket_send
};

static void pluto_task(int sig, void *arg,int len);

void  pluto_init(void)
{
	socket_init((nwk_adapter_t*)&myadapter);
	pluto_pre_init();
	osStartReloadTimer(1000,pluto_task,1,NULL);
}
static void  pluto_task(int sig, void *arg,int len)
{
	uint32 ret,cgreg;
	osLogD(0,"pluto_task\r\n");
	ret = RIL_NW_GetEGPRSState(&cgreg);
	if((cgreg == NW_STAT_REGISTERED)||(cgreg == NW_STAT_REGISTERED_ROAMING))
	{
		osLogI(DBG_UDP_SOC,"socket_task: GPRS State OK and start to initialization udp socket:%d \r\n",cgreg);
		pluto_start_server(0);
		osDeleteTimer(pluto_task,1);
	}
}

void  pluto_registe_led(led_blink_cb_t cb)
{
	blink_led= cb;
}
void  pluto_led_blink(int num, int htime, int ltime)
{
	if(blink_led!=NULL)
	{
		blink_led(num,htime,ltime);
	}
}



