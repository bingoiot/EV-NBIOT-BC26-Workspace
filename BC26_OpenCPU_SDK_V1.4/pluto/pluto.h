/*
 * jifan.h
 *
 *  Created on: 2016年3月17日
 *      Author: Administrator
 */

#ifndef PLUTO_H__
#define PLUTO_H__

#include "cJSON.h"
#include "osCore.h"

typedef enum
{
	op_succeed 		= 0x00,
	op_failed		,//1
	op_error		,//2
	op_invalid		,//3
	op_permit_denied ,//4
	op_repeat		,//5
	op_no_port		,//6
	op_no_file		,//7
	op_read_error	,//8
	op_write_error 	,//9
	op_vmInvalid	,//10
	op_vmRun		,//11
	op_vmPause		,//12
	op_vmStop		,//13
	op_vmFinished	,//14
	op_vmNoTask		,//15
	op_vmSysTick	,//16
	op_vmTest		,//17
	op_vmJoined		,//18
	op_vm_param		,//19
}op_state_t;

typedef enum
{
	protocol_udp = 0,
	protocol_tcp = 1,
	protocol_local_loop = 2,
}protocol_t;

typedef enum
{
	login_state_stop,
	login_state_start,
	login_state_online,
	login_state_offline,
	login_state_login_failed,
	login_state_logout_failed,
}login_state_t;


typedef enum
{
	cmd_invalide		=  	0x00,
	cmd_read			=	0x01,
	cmd_write 			= 	0x02,
	cmd_del				= 	0x03,
	cmd_toggle			= 	0x04,
	cmd_alarm			= 	0x05,
	cmd_readname		= 	0x06,
	cmd_rename 			= 	0x07,
	cmd_readstate		=   0x08,
	cmd_writestate		=	0x09,
	cmd_readCfg			= 	0x0A,
	cmd_writeCfg		= 	0x0B,
	cmd_beacon			= 	0x0C,
	cmd_notify			=	0x0D,
	cmd_return			=  	0x80,
}default_command_t;

typedef enum
{
	keyID_default 	= 0x00,
	keyID_sn		= 0x01,
	keyID_admin		= 0x02,
	keyID_common	= 0x03,
	keyID_guest		= 0x04,
	keyID_taget		= 0x08,
}keyID_t;


typedef enum
{
	dtype_unkown = 0,
	dtype_byte,
	dtype_int ,
	dtype_float,
	dtype_string,
}data_type_t;

typedef enum
{
	dev_type_invalide	= 0x00,
	dev_type_gateway 	= 0x01,
	dev_type_route		= 0x02,
	dev_type_device		= 0x03,
	dev_type_lowenergy	= 0x04,
	dev_type_nbiot_normal = 0x05,
	dev_type_nbiot_lowenergy = 0x06
}device_type_t;

typedef enum
{
	option_default				= 0x00,//no option
	option_skip_scene 			= 0x01,//send message and skip trigger scene
	option_skip_route  			= 0x02,//send message without route device
	option_no_record 			= 0x04,//send message no record
	option_no_delay				= 0x08,//send message no delay
	option_Qos_1				= 0x10,//send message and try 24 hour(Will be supported in the future)
	option_Qos_2				= 0x20,//send message and try 7 days(Will be supported in the future)
	option_msg_from_svr 		= 0x80,//receive message no from local
}send_option_t;

typedef struct
{
	int 	version;
	char 	*MFG;
	char	*bin_name;
	char 	*device_name;
	int	 	device_type;
	int		guestLock;
	int  	shareLock;
	int		localLock;
	int		remoteLock;
	int		zone;
	int		aIDNum;
	uint32	*aID;
}device_describe_t;

typedef struct device
{
	uint8	addr[8];
	uint8 	keyID;
	uint8	port;
	uint8	option;
}address_t;

typedef struct command
{
	uint32 	aID;
	uint8 	cmd_id;
	uint8 	option;
	uint8	reserve[2];
}command_t;

typedef struct
{
	uint32	product_ID;//产品种类编码，HA类产品编码为1
	uint32  vendor_ID;
	char	*product_date;
	char	*password;
	uint8 	*sn;
	uint8	*addr;
	char	*url;
	char	*server_ipv4;
	uint16	server_udp_port;
	uint16	server_tcp_port;
	uint16	local_udp_port;
	uint16	version;//产品初始版本
}pdo_server_t;

typedef int (*socket_udp_open)(uint16 local_port);
typedef int (*socket_tcp_open)(uint32 service_ip, uint16 service_port);
typedef int (*socket_udp_close)(void);
typedef int (*socket_tcp_close)(void);
typedef int (*socket_udp_output)(uint32 ip, uint16 port, uint8 *pdata, uint16 len);
typedef int (*socket_tcp_output)(uint8 *pdata,uint16 len);

typedef struct
{
	socket_udp_open 	udp_open;
	socket_tcp_open		tcp_open;
	socket_udp_close	udp_close;
	socket_tcp_close	tcp_close;
	socket_udp_output 	udp_output;
	socket_tcp_output	tcp_output;
}nwk_adapter_t;

typedef void 	(*aps_recieve_message_t)(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len);
typedef void 	(*aps_send_status_t)(address_t *dst,uint8 seq, command_t *cmd, uint8 state);
typedef void 	(*login_state_cb_t)(uint8 state);
typedef void 	(*permit_join_cb_t)(uint8 state, uint16 duration);
typedef void 	(*device_indcation_cb_t)(uint8*src);
typedef void 	(*delete_port_cb_t)(uint8 port);

typedef struct
{
	aps_recieve_message_t	recieve_msg;
	aps_send_status_t		send_status;
}aps_listener_t;

/************basic function********/
extern void 		pluto_pre_init(void);
extern osState 		pluto_start_server(uint32 ip);
extern void 		socket_init(nwk_adapter_t *adapter);
extern void 		socket_udp_input(uint32 ip, uint16 port, uint8 *pdata, uint16 len);
extern void 		socket_tcp_input(uint8 *pdata, uint16 len);


extern void 		login_set_user_psw(uint8* user, uint8 *psw);
extern osState  	login_start(void);
extern osState      login_stop(void);
extern osState      login_logout(void);
extern void  		login_set_state_change_cb(login_state_cb_t cb);
extern uint8 		login_get_state(void);

extern void 		aps_set_listener(aps_listener_t *listener);
extern osState  	aps_req_send_command(address_t *dst,uint8 seq,command_t* cmd, uint8 *pdata, uint32 len,uint8 send_option);
extern osState  	aps_req_send_status(address_t *dst,uint8 seq,command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
extern osState  	aps_req_report_command(uint8 port, uint8 seq,command_t *cmd,uint8 *pdata, uint32 len ,uint8 send_option);
extern osState  	aps_req_report_status(uint8 port,uint8 seq,command_t* cmd,uint8 state, uint8 *pdata, uint32 len,uint8 send_option);
/*******pdo operation **********/

extern osState 		pdo_set_device_describe(device_describe_t *desc);
extern osState  	pdo_save_obj_valueint(char *obj, int value);
extern uint8  		pdo_read_port_num(void);
extern uint8*  		pdo_read_port_list(void);
extern	uint8 		pdo_get_free_port(void);
extern uint8 		pdo_port_is_exist(uint8 port);
extern char* 		pdo_read_port_describe(uint8 port);
extern osState  	pdo_update_port_describe(char *describe);
extern osState 		pdo_delete_port(uint8 port_num);
extern osState 		pdo_register_port(uint8 port_num,uint32 applicationID, uint32 *attr_list, uint8 attr_num, uint8 force);

extern osState 		pdo_port_set_value(uint8 port,char *obj, int value);
extern osState 		pdo_port_set_string(uint8 port, char *obj, char *str);
extern uint32 		pdo_port_get_value(uint8 port, char *obj);
extern char* 		pdo_port_get_string(uint8 port, char *obj);
extern uint8 		pdo_count_port_by_value(char *obj,int value);
extern uint8 		pdo_count_port_by_string(char *obj,char *str);
extern uint8 		pdo_get_port_by_value_index(char *obj,int value, uint8 index);
extern uint8 		pdo_get_port_by_string_index(char *obj,char *str, uint8 index);
extern uint8	 	pdo_get_port_by_value2(char *obj1,int value1, char *obj2, int value2);

extern void 		pdo_set_permit_join_listener(permit_join_cb_t cb);
extern void 		pdo_set_delete_port_listener(delete_port_cb_t cb);
extern void 		pdo_req_send_device_annouce(device_indcation_cb_t cb);
extern osState  	pdo_req_send_beacon(uint8 keyID, uint8 *addr, uint8 *pdata, uint32 len);
extern osState  	pdo_req_read_unixtime(uint8 keyID, uint8 *addr);
extern uint8 		pdo_get_port_permit_join_flag(void);

extern uint16 		pluto_get_seq(void);

extern uint8* 		pluto_get_local_key(uint8 keyID);
extern uint8* 		pluto_get_default_key(void);
extern uint8* 		pluto_get_adminkey(void);
extern uint8* 		pluto_get_comkey(void);
extern uint8* 		pluto_get_guestkey(void);
extern void 		pluto_set_adminkey(uint8* key);
extern void 		pluto_set_comkey(uint8* key);
extern void 		pluto_set_guestkey(uint8* key);
extern uint8 		pluto_get_device_lqi(void);

/*******device key operation*****/
extern int  		pdo_clear_device_keys(void);
extern int  		pdo_add_device_key(uint8 *addr,uint8 keyID,uint8 *key);
extern uint8*		pdo_get_devcie_key(uint8 *addr, uint8 keyID);
extern int  		pdo_remove_device_keys(uint8 *addr);
extern int  		pdo_remove_device_key(uint8 *addr,uint8 keyID);

/*********file system***************/
extern char*		pluto_read_text_file(char *fname);
extern osState  	pluto_write_text_file(char *fname,char *str);
extern uint32 		pluto_get_file_length(char *fname);
extern uint32 		pluto_read_file(char *fname, uint8 *pdata, uint32 len);
extern osState 		pluto_write_file(char *fname, uint8 *pdata, uint32 len);
extern char*		pluto_get_file_name(char *filter, uint32 id);
extern osState 		pluto_delete_file(char *fname);
extern osState 		pluto_delete_all_file(void);
extern osState 		pluto_disk_format(void);
extern osState  	pluto_rename(char *pname,char *newname);

/*********factory settings******/
extern osState 		pdo_write_server(pdo_server_t *pserver);
extern uint32 		pdo_get_vender_id(void);
extern uint32 		pdo_get_product_id(void);

/*******scene operation*************/
extern void		 	Scene_CreateTask(char *task,char *name,osState isboot);
extern void 		Scene_DelTask(char *name);
extern void			Scene_Join(char *tsk0, char *tsk1);
extern void 		Scene_SendParameter(char *name, char *arg);
extern cJSON*		Scene_CreatBody(void);
extern cJSON*		Scene_AddWhile(cJSON *body, char *reason);
extern cJSON*		Scene_Addif(cJSON *body, char *what, char *reason);
extern cJSON*		Scene_AddReadFor(cJSON *body, char *reason);
extern cJSON*		Scene_AddException(cJSON *body);
extern cJSON*		Scene_AddElse(cJSON *body);
extern void 		Scene_AddAction(cJSON *body, char *fuc, char *str);





#endif /* JIFAN_JIFAN_H_ */
