/*
 * jf_core.h
 *
 *  Created on: 2016��1��6��
 *      Author: Administrator
 */

#ifndef OS_CORE_H_
#define OS_CORE_H_

#include "osType.h"

typedef struct
{
	int 	wday;
    uint16  year;
    uint8   mon;
    uint8   day;

    uint8   hour;
    uint8   min;
    uint8   sec;
    uint8	rstat;
} osLocalTime_t;

typedef struct
{
	void 		*next;
	uint32		cnt;
	uint32		reload;
	task_t		fuc;
	int			sig;
	int			len;
	void		*arg;
}osTimer_t;


#define os_get_u32_hh(x)	((x>>24)&0x000000FF)
#define os_get_u32_h(x)		((x>>16)&0x000000FF)
#define os_get_u32_l(x)		((x>>8)&0x000000FF)
#define os_get_u32_ll(x)	((x)&0x000000FF)

#define os_get_u16_h(x)		((x>>8)&0x00FF)
#define os_get_u16_l(x)		((x)&0x00FF)

void 	osSrand(uint32 seek);
uint32 	osRand (void);
void 	osRandbytes(uint8 *d, int len);

void  	*osMemcpy(void *dst, const void *src, unsigned long ulcount);
int 	osMemcmp(void *s, void *d, int len);
void  	*osMemset(void *dst, int val, unsigned long ulcount);
int 	osMemchkz(void *s, int len);

void   	osStrcpy(const void *d,const void *s);
void   	osStrcat(void *d,void *s);
int  	osStrcmp(const char *source,const char *dest);
int  	osStrncmp(const char *first,const char *last,int count);
int  	osStrlen(const char * str);
int  	osStrnlen(const char *s, int count);
char  	*osStrchr(const char *str, int ch);
char  	*osStrstr(char *str0,char *str1);
char  	*osCaseStrstr(char *str0,char *str1);

int  	osToupper(int c);
uint64 	osBtoU64(uint8 *pdata);
uint32 	osBtoU32(uint8 *pdata);
uint16 	osBtoU16(uint8 *pdata);
void 	osU64toB(uint8 *pdata, uint64 val);
void 	osU32toB(uint8 *pdata, uint32 val);
void 	osU16toB(uint8 *pdata, uint16 val);
char 	*osBtoStr(uint8 *pdata, int len);
/***
 * s: covert to hex string
 * slen:max char lenth to be covert
 * fill: fill char
 * b: covert bytes
 * blen: max bytes to be covert
 */
int	 	osBtoHexStr(char *s,char split,uint8 *b, int blen);
int 	osHexStrtoB(uint8 *d,int dlen,char *s,char split);

uint32  os_inet_addr(char *ip);
int 	os_addr_inet(char *ipv4, uint32 ip);
char  	osBtoA(uint8 x);
uint8  	osAtoB(char c);

uint32 	osSubAbs(uint32 s,uint32 n);
uint32 	osSubLoop32(uint32 s, uint32 n);
uint16  osGetCheckSum16(uint8 *pdata, int len);
//crc16 x16+x15+x2+1
void  	osGenRandomArrary(uint8 *p,int len);
#define osGetCRC16(pdata, len)		osCRC16(0,pdata,len)
uint16  osCRC16(uint16 crc, uint8 *pdata, int len);
uint8  	osGetXOR(uint8 *pdata, int len);

int  	osAtoI(char *str);
char  	*osItoA(int n, char * chBuffer);
int 	osFtoA(char *str, float num, int n);        //n��ת���ľ��ȣ������ַ���'.'���м�λС��
float 	osAtoF(const char *str);
int  	osSkipAtoI(const char **s);

#define osSprintf		sprintf_
#define osGetMin(x,y)	((x>y)?y:x)

#define os_htons			//Ql_htons
#define os_ntohs			//Ql_ntohs

#define osSleep(x)		Ql_Sleep(x);

extern osState	 		osInit(void);
extern osState 			osSendMessage(task_t fuc, int sig, void *arg,int len);
extern void 			osTickUpdata(uint32 ticks);

extern osState  		osStartSuperTimer(uint32 tim, task_t fuc, int sig, void *arg,int len);
extern osState  		osStartSuperReloadTimer(uint32 tim, task_t fuc, int sig, void *arg,int len);
extern osState  		osStartTimer(uint32 tim, task_t fuc, int sig, void *arg);
extern osState  		osStartReloadTimer(uint32 tim, task_t fuc, int sig, void *arg);
extern osState 			osAddTimer(uint32 tim,uint32 reload, task_t fuc,int sig,void *arg,int len);
extern osState 			osDeleteTimer(task_t fuc, int sig);
extern void 			osTimerInfo(void);

extern unsigned long  	osLocalToUnix(int zone,osLocalTime_t *date_time);
extern int 				osUnixtoLocal (int zone, osLocalTime_t *tp,uint64 t);

extern uint32  			osGetmsSecond(void);
extern uint32  			osGetmsTimestamp(void);
extern uint32  			osGetUnixTime(void);
extern void  			osSetUnixTime(uint32 sec);
#define 	osMalloc(x)		Ql_MEM_Alloc(x)
#define		osFree(x)		Ql_MEM_Free(x)

#define  	osEntercritical() 	_EnterCritical()//	vTaskSuspendAll()//vPortEnterCritical()
#define  	osExitcritical()	_ExitCritical()//xTaskResumeAll()//vPortExitCritical()

#define DBG_UDP_SOC			0
#define DBG_TCP_SOC			0
#define DBG_SN				0
#define DBG_FS				0
#define DBG_FS_LEVEL1		0
#define DBG_FS_LEVEL2		0
#define DBG_COMMON			1

#define DBG_ERROR			1

void os_printf(const char *fmt, ...);
void osPrint2b(char *s,uint8 *pdata, int len);

#define osPrintb(fmt, ...)\
		do{ \
			if(fmt)		\
			osPrint2b(__VA_ARGS__);\
		}while(0)

#define osPrintf(fmt,...)\
	do{ \
		if(fmt)		\
		os_printf(__VA_ARGS__);\
	}while(0)

#define osLogI(fmt, ...)\
		do{ \
			if(fmt)		\
			{os_printf("log -I %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);\
			os_printf(__VA_ARGS__);}\
		}while(0)

#define osLogD(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{os_printf("log -D %s %s() line:%d :",__FILE__, __FUNCTION__,__LINE__);\
				os_printf(__VA_ARGS__);}\
			}\
		}while(0)

#define osLogE(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{os_printf("log -E %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);\
				os_printf(__VA_ARGS__);}\
			}\
		}while(0)

#define osLogB(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{os_printf("log -B %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);osPrint2b(__VA_ARGS__);}\
			}\
		}while(0)

typedef void (*dbg_print_t)(uint8 *data,int len);

extern osState os_dbg_init(dbg_print_t *fuc);

#endif /* OSCORE_H_ */
