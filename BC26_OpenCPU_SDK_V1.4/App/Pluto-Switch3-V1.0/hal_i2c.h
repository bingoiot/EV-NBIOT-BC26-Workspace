/*
 * hal_i2c.h
 *
 *  Created on: 2019年5月1日
 *      Author: Administrator
 */

#ifndef APP_GATEWAY_ZB_V01_HAL_I2C_H_
#define APP_GATEWAY_ZB_V01_HAL_I2C_H_

#include "osCore.h"

#define I2C_NACK		1
#define I2C_ACK			0

void Hal_I2C_Init(void);
/*================================================================
【名 称】void I2CStart(void)
【功 能】I2C启动信号
【备 注】SCL、SDA同为高，SDA跳变成低之后，SCL跳变成低
================================================================*/
void Hal_I2C_Start(void);
/*================================================================
【名 称】void I2CStop(void)
【功 能】I2C停止信号
【备 注】SCL、SDA同为低，SCL跳变成高之后，SDA跳变成高
================================================================*/
void Hal_I2C_Stop(void);
/*================================================================
【名 称】uint8 I2CWRByte(uint8 WriteData)
【功 能】I2C写一个字节数据，返回ACK或者NACK
【备 注】从高到低，依次发送
================================================================*/
uint8 Hal_I2C_WriteByte(uint8 WriteData);
/*================================================================
【名 称】uint8 I2CRDByte(uint8 AckValue)
【功 能】I2C读一个字节数据，入口参数用于控制应答状态，ACK或者NACK
【备 注】从高到低，依次接收
================================================================*/
uint8 Hal_I2C_ReadByte(uint8 AckValue);

#endif /* APP_GATEWAY_ZB_V01_HAL_I2C_H_ */
