/*
 * udp_socket.c
 *
 *  Created on: 2016Äê5ÔÂ4ÈÕ
 *      Author: lort
 */

#include "osCore.h"
#include "external_include.h"
#include "pluto_entry.h"
#include "pluto.h"
#include "ril_socket.h"

typedef enum
{
	conn_invalide,
	conn_start,
	conn_succeed,
}ConnState;

#define STRING_IP_LENGTH 	64//200.200.200.200
#define SOCKET_BUF_SIZE		500

static void 			socket_task(int sig,void *arg,int len);
static osState 			socket_open(const char *ip, uint16 port);

static uint8 				udp_active = osTrue;
static uint8				state = 0;
static uint8				SvrIP[64];
static uint16				SvrPort = 0;
static ST_Socket_Param_t  	socket_param_t = {0,0,0,NULL,0,0,0,0};
static int					socketID = -1;
int udp_socket_deinit(void)
{
	if(socketID>=0)
	{
		RIL_SOC_QICLOSE(socketID);
		socketID = -1;
	}
	else
	{
		osLogI(DBG_ERROR,"udp_socket_deinit socket did not open\r\n");
	}
	return osSucceed;
}
int udp_socket_init(uint32 ip,uint16 port)
{
	char sip[64];
	if(socketID<0)
	{
		os_addr_inet(sip,ip);
		osStrcpy(SvrIP,sip);
		SvrPort = port;
		osStartReloadTimer(1000,socket_task,1,NULL);
		state=conn_invalide;
		osLogI(DBG_UDP_SOC,"udp_socket_init:%s:%d\r\n",SvrIP,SvrPort);
	}
    return 0;
}
int udp_socket_send(uint32 ip, uint16 port, uint8 *pdata, uint16 len)
{
	uint8 ATBuf[2048];
	if(socketID>=0)
	{
		len = (len>500)?500:len;
		osBtoHexStr(ATBuf,0x00,pdata,len);
		osLogB(DBG_UDP_SOC,"udp_socket_send:",pdata,len);
		RIL_SOC_QISENDEX(socketID, len, ATBuf);
	}
	else
	{
		osLogI(DBG_ERROR,"udp_socket_send socket did not open\r\n");
	}
	return -1;
}
void udp_enable(osState state)
{
	osLogI(DBG_UDP_SOC,"udp_enable: state:%d  \r\n",state);
	udp_active = state;
}
static int udp_socket_receive(uint8 *pdata, uint16 len)
{
	uint8 ATBuf[2048];
	int ret = -1;
	s32	 rlen,slen;
	if(socketID>=0)
	{
		if((ret = RIL_SOC_QIRD(socketID, len,&slen,&rlen,ATBuf))==RIL_AT_SUCCESS)
		{
			if(slen>0)
			{
				osHexStrtoB(pdata,slen,ATBuf,0x00);
			}
			return slen;
		}
	}
	else
	{
		osLogI(DBG_ERROR,"udp_socket_send socket did not open\r\n");
	}
	return -1;
}
static osState socket_open(const char *ip, uint16 port)
{
	int ret;
	socket_param_t.contextID = 1;
	socket_param_t.connectID = 0;
	socket_param_t.access_mode = SOCKET_ACCESS_MODE_BUFFER;
	socket_param_t.remote_port = port;
	socket_param_t.local_port = 1235;
	socket_param_t.service_type =SOCKET_SERVICE_TYPE_UDP;
	socket_param_t.protocol_type = SOCKET_PROTOCOL_TYPE_IPV4;

	socket_param_t.address = osMalloc(sizeof(u8)*STRING_IP_LENGTH);
	if(socket_param_t.address!=NULL)
	{
		osMemset(socket_param_t.address,0,STRING_IP_LENGTH);
        osStrcpy(socket_param_t.address,ip);
	}
	osLogI(DBG_UDP_SOC,"socket_open:%s:%d\r\n",socket_param_t.address,socket_param_t.remote_port);
    ret = RIL_SOC_QIOPEN(&socket_param_t);
	if(ret>=0 && ret<=4)
    {
        osLogI(DBG_UDP_SOC,"<--Open socket successfully,socketid=%d-->\r\n",ret);
    }
	else
    {
		osLogI(DBG_UDP_SOC,"<--Open socket failure,error=%d.-->\r\n",ret);
		ret = RIL_SOC_QICLOSE(socket_param_t.connectID);
		ret = -1;
    }
     Ql_MEM_Free(socket_param_t.address);
	 socket_param_t.address = NULL;
	return ret;
}
static void socket_task(int sig,void *arg,int len)
{
	int 	ret;
	uint32 	cgreg;
	osLogI(0,"socket_task:sig:%d, state:%d \r\n",sig,state);
	if(sig==1)
	{
		ret = RIL_NW_GetEGPRSState(&cgreg);
		if((cgreg == NW_STAT_REGISTERED)||(cgreg == NW_STAT_REGISTERED_ROAMING))
		{
			osLogI(DBG_UDP_SOC,"socket_task: GPRS State OK and start to initialization udp socket:%d \r\n",cgreg);
			if(state==conn_invalide)
			{
				state = conn_start;
				osStartReloadTimer(100,socket_task,2,NULL);
				osStartReloadTimer(10000,socket_task,1,NULL);
			}
		}
		else
		{
			osLogI(DBG_ERROR,"socket_task: GPRS State no ready :%d \r\n",cgreg);
			state = conn_invalide;
			osDeleteTimer(socket_task,2);
			osStartReloadTimer(1000,socket_task,1,NULL);
		}
	}else if(sig==2)
	{
		switch(state)
		{
		case conn_invalide:
			break;
		case conn_start:
			if((ret = socket_open(SvrIP,SvrPort))>=0)
			{
				RIL_SOC_QICFG_FORMAT(1,1);
				state = conn_succeed;
				socketID = ret;
				osLogI(DBG_UDP_SOC,"create socket succeed id:%d \r\n",socketID);
			}
			else
			{
				osLogI(DBG_ERROR,"create socket failed\r\n");
				socketID = -1;
			}
			break;
		case conn_succeed:
			osDeleteTimer(socket_task,2);
			break;
		default:
			break;
		}
	}
}
void proc_udp_task(s32 taskId)
{
	uint8 rxbuffer[SOCKET_BUF_SIZE];
	int ret;
	while(1)
	{
		while(socketID<0)
		{
			osSleep(1000);
		}
		ret = udp_socket_receive(rxbuffer,SOCKET_BUF_SIZE);
		if(ret>0)
		{
			osLogB(1,"socket_udp_input:",rxbuffer,ret);
			socket_udp_input(nwk_get_server_ipv4(), nwk_get_server_port(protocol_udp),rxbuffer,ret);
		}
		osSleep(50);
	}
}

