
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   main.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This app demonstrates how to send AT command with RIL API, and transparently
 *   transfer the response through MAIN UART. And how to use UART port.
 *   Developer can program the application based on this example.
 * 
 ****************************************************************************/
#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_timer.h"
#include "osCore.h"
#include "tcp_socket.h"
#include "hal_spi.h"
#include "w25qxx.h"
#include "pluto_file_system.h"
#include "bc26_init.h"
#include "hal_uart.h"
//void test();
extern void  app_task_init(void);
void proc_main_task(s32 taskId)
{
	/** init debug uart first**/
	hal_uart_init();
	os_dbg_init(hal_uart_send);

    osLogI(1,"proc_main_task: task test osLogI \r\n");
    // START MESSAGE LOOP OF THIS TASK
    Ql_RIL_Initialize();
    bc26_init();
    app_task_init();
    while(TRUE)
    {
    	//osLogI(1,"main task tick timestamp: %d \r\n",osGetmsTimestamp());
    	osSleep(3000);
    }
}

