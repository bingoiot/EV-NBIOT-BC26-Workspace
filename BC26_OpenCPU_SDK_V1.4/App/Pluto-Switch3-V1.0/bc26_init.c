/*
 * bc26_init.c
 *
 *  Created on: 2019Äê3ÔÂ20ÈÕ
 *      Author: zigbee
 */

#include "osCore.h"
#include "bc26_init.h"

#define DBG_BC26	1
#define AT_INIT_TIMEOUT			300
static uint8	state = 0;

const char *ATTest="AT";
const char *ATQueryCFun="AT+CFUN?";
const char *ATQueryCPin="AT+CIMI";
const char *ATQueryPDN="AT+QCGDEFCONT?";
const char *ATSetPDN="AT+QCGDEFCONT=\"IP\",\"spe.inetd.vodafone.nbiot\"";
const char *ATResetModule = "AT+QRST=1";
const char *ATReportNetworkState="AT+CEREG=1";
const char *ATReportConnectState="AT+CSCON=1";
const char *ATQueryAttached="AT+CGATT?";
const char *ATQueryRSSI="AT+CESQ";
const char *ATQueryState="AT+QENG=0";
const char *ATActivePDP = "AT+CGACT=1,1";
const char *ATDisableSleep = "AT+SM=LOCK";
const char *ATENABLEDBGHSL="AT+EPORT=1,uls,2";
const char *ATENABLEDBGGKI="AT+EPORT=1,emmi,1";
const char *ATSeteDRxMode = "AT+CEDRXS";

static void bc26_task(int sig,void *arg,int len);
static s32 bc26_receive_process(char* line, u32 len, void* userData);
static s32 bc26_receive_rssi_cb(char* line, u32 len, void* userData);

void bc26_init(void)
{
	osStartReloadTimer(1000,bc26_task,1,NULL);
	osStartReloadTimer((5*60*1000),bc26_task,2,NULL);//time to restart
}

osState bc26_isok(void)
{
	if(state>10)
		return osTrue;
	else
		return osFalse;
}
osState bc26_setDRxMode(eDRxMode_t mode, uint8 type, char *eDRx_Value)
{
	char ATBuf[64];
	//Ql_sprintf(ATBuf,"%s=?",ATSeteDRxMode);
	//osLogI(DBG_BC26,"bc26_setDRxMode :%s \r\n",ATBuf);
	//return Ql_RIL_SendATCmd((char*)ATBuf,osStrlen(ATBuf),bc26_receive_process,NULL,AT_INIT_TIMEOUT);

	Ql_sprintf(ATBuf,"%s=%d,%d,\"%s\"\r\n",ATSeteDRxMode,mode,type,eDRx_Value);
	osLogI(DBG_BC26,"bc26_setDRxMode :%s \r\n",ATBuf);
	return Ql_RIL_SendATCmd((char*)ATBuf,osStrlen(ATBuf),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
}
int bc26_query_rssi(void)
{
	int rssi = 0;
	char ATBuf[64];
	Ql_sprintf(ATBuf,"%s\r\n",ATQueryRSSI);
	osLogI(DBG_BC26,"bc26_query_rssi :%s \r\n",ATQueryRSSI);
	Ql_RIL_SendATCmd((char*)ATBuf,osStrlen(ATBuf),bc26_receive_rssi_cb,&rssi,AT_INIT_TIMEOUT);
	return rssi;
}
static void bc26_task(int sig,void *arg,int len)
{
	if(sig==1)
	{
		osLogI(DBG_BC26,"bc26_task state:%d \r\n",state);
		switch(state)
		{
			case 0:
				osStartReloadTimer(100,bc26_task,1,NULL);
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATTest);
				Ql_RIL_SendATCmd((char*)ATTest,osStrlen(ATTest),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 1:
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryCFun);
				Ql_RIL_SendATCmd((char*)ATQueryCFun,osStrlen(ATQueryCFun),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 2:
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryCPin);
				Ql_RIL_SendATCmd((char*)ATQueryCPin,osStrlen(ATQueryCPin),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 3:
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryPDN);
				Ql_RIL_SendATCmd((char*)ATQueryPDN,osStrlen(ATQueryPDN),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 4://req Set PDN profile
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATSetPDN);
				Ql_RIL_SendATCmd((char*)ATSetPDN,osStrlen(ATSetPDN),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 5://reset module
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATResetModule);
				Ql_RIL_SendATCmd((char*)ATResetModule,osStrlen(ATResetModule),NULL,NULL,AT_INIT_TIMEOUT);
				state = 0;
				break;

			case 6://set auto report network state
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATReportNetworkState);
				Ql_RIL_SendATCmd((char*)ATReportNetworkState,osStrlen(ATReportNetworkState),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 7://set auto report signal connection state
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATReportConnectState);
				Ql_RIL_SendATCmd((char*)ATReportConnectState,osStrlen(ATReportConnectState),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 8://
				osStartReloadTimer(5000,bc26_task,1,NULL);
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryRSSI);
				Ql_RIL_SendATCmd((char*)ATQueryRSSI,osStrlen(ATQueryRSSI),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 9://
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryState);
				Ql_RIL_SendATCmd((char*)ATQueryState,osStrlen(ATQueryState),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 10://Query network attached
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATDisableSleep);
				//osStartReloadTimer(1000,bc26_task,1,NULL);
				Ql_RIL_SendATCmd((char*)ATDisableSleep,osStrlen(ATDisableSleep),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			case 11://Query network attached
				osLogI(DBG_BC26,"AT Send Cmd:%s \r\n",ATQueryAttached);
				//osStartReloadTimer(1000,bc26_task,1,NULL);
				Ql_RIL_SendATCmd((char*)ATQueryAttached,osStrlen(ATQueryAttached),bc26_receive_process,NULL,AT_INIT_TIMEOUT);
				break;
			default:
				osDeleteTimer(bc26_task,1);
				break;
		}
	}
	else if(sig==2)
	{
		if(state<=10)
		{
			osLogI(DBG_BC26,"BC26 Init Timeout and send reset command:%s \r\n",ATResetModule);
			Ql_RIL_SendATCmd((char*)ATResetModule,osStrlen(ATResetModule),NULL,NULL,AT_INIT_TIMEOUT);
			state = 0;
		}
	}

}
static s32 bc26_receive_rssi_cb(char* line, u32 len, void* userData)
{
	char *head;
	int 	temp = 0;
	osLogI(DBG_BC26,"bc26_receive_rssi_cb:%s \r\n",line);
	if((head = Ql_RIL_FindString(line, len, "+CESQ:"))!=NULL)
	{
		head += 7;
		temp = osAtoI(head);
		if(userData!=NULL)
			*(int*)userData = temp;
		state ++;
		osLogI(DBG_BC26,"query rssi:%d \r\n",temp);
	}
	return RIL_ATRSP_SUCCESS;
}
static s32 bc26_receive_process(char* line, u32 len, void* userData)
{
	char *head,*p;
	osLogI(1,"line:%s \r\n",line);
	osLogB(DBG_BC26,"user data:",(uint8 *)userData,len);
	switch(state)
	{
		case 0:
			if((head = Ql_RIL_FindString(line, len, "OK"))!=NULL)
			{
				osLogI(DBG_BC26,"System State: OK\r\n");
				state++;
			}
			else
			{
				osLogI(DBG_ERROR,"System Error!\r\n");
			}
			break;
		case 1:
			if((head = Ql_RIL_FindString(line, len, "+CFUN:"))!=NULL)
			{
				head+=7;
				osLogI(DBG_BC26,"Function State:%s\r\n",head);
				state++;
			}
			break;
		case 2:
			if((head = Ql_RIL_FindString(line, len, "ERROR"))!=NULL)
			{
				osLogI(DBG_BC26,"Please Input USIM Card !\r\n",head);
			}
			else
			{
				head = line+2;
				osLogI(DBG_ERROR,"USIM Card :%s\r\n",head);
				state++;
			}
			break;
		case 3:
			if((head = Ql_RIL_FindString(line, len, "+QCGDEFCONT:"))!=NULL)
			{
				head+=13;
				if((p = osStrstr(head,"IPV4V6"))!=NULL)
				{
					osLogI(DBG_BC26,"Need to write PDN Profile:%s\r\n",head);
					state = 4;//try to write PDN profile;
				}
				else
				{
					osLogI(DBG_BC26,"PDN Setting:%s\r\n",head);
					state = 6;
				}
			}
			break;
		case 4:
			if((head = Ql_RIL_FindString(line, len, "OK"))!=NULL)
			{
				osLogI(DBG_BC26,"PDN Write Setting:%s\r\n",head);
				state = 5;//reset nbiot module
			}
			break;
		case 5://reset handler
			break;
		case 6://auto report network state
			if((head = Ql_RIL_FindString(line, len, "OK"))!=NULL)
			{
				osLogI(DBG_BC26,"System State: OK\r\n");
				state++;
			}
			break;
		case 7://auto report signal connect state
			if((head = Ql_RIL_FindString(line, len, "OK"))!=NULL)
			{
				osLogI(DBG_BC26,"System State: OK\r\n");
				state++;
			}
			break;
		case 8://query rssi
			if((head = Ql_RIL_FindString(line, len, "+CESQ:"))!=NULL)
			{
				head += 7;
				osLogI(DBG_BC26,"query rssi:%s \r\n",head);
				state ++;
			}
			break;
		case 9://query module state
			if((head = Ql_RIL_FindString(line, len, "+QENG:"))!=NULL)
			{
				head += 7;
				osLogI(DBG_BC26,"query rssi:%s \r\n",head);
				state ++;
			}
			break;
		case 10:
			if((head = Ql_RIL_FindString(line, len, "OK"))!=NULL)
			{
				osLogI(DBG_BC26,"Set Disable Sleep State: OK\r\n");
				state++;
			}
			break;
		case 11://auto report signal connect state
			if((head = Ql_RIL_FindString(line, len, "+CGATT: 1"))!=NULL)
			{
				osLogI(DBG_BC26,"System State: OK\r\n");
				state++;
			}
			break;
		default:
			osLogI(DBG_BC26,"AT Setting profile finished ! \r\n");
			osDeleteTimer(bc26_task,1);
			break;
	}
	return RIL_ATRSP_SUCCESS;
}
