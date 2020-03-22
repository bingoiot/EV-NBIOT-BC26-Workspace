/***********************************************************************************************
*
*
*
*
*
*
*
*
***************************************************************************************************/
#include "osCore.h"

#define I2C_SCL_PIN		PINNAME_GPIO4			 // 连接到SCL时钟线的GPIO
#define I2C_SDA_PIN		PINNAME_GPIO5			 // 连接到SDA数据线的GPIO

#define I2C_NACK		1
#define I2C_ACK			0

static uint8 I2C_SCL_READ(void);
static uint8 I2C_SDA_READ(void);
static uint8 I2C_SCL_1(void);
static uint8 I2C_SCL_0(void);
static uint8 I2C_SDA_1(void);
static uint8 I2C_SDA_0(void);
static void I2C_Delay (uint8 t);

static uint8 I2C_SCL_READ(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SCL_PIN,PINDIRECTION_IN);
		if(Ql_GPIO_GetLevel(I2C_SCL_PIN))
			ret = 1;
	}while(0);
	return ret;
}
static uint8 I2C_SDA_READ(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SDA_PIN,PINDIRECTION_IN);
		if(Ql_GPIO_GetLevel(I2C_SDA_PIN))
			ret = 1;
	}while(0);
	return ret;
}
static uint8 I2C_SCL_1(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SCL_PIN,PINDIRECTION_OUT);
		Ql_GPIO_SetLevel(I2C_SCL_PIN,1);
	}while(0);
	return ret;
}
static uint8 I2C_SCL_0(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SCL_PIN,PINDIRECTION_OUT);
		Ql_GPIO_SetLevel(I2C_SCL_PIN,0);
	}while(0);
	return ret;
}
static uint8 I2C_SDA_1(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SDA_PIN,PINDIRECTION_OUT);
		Ql_GPIO_SetLevel(I2C_SDA_PIN,1);
	}while(0);
	return ret;
}
static uint8 I2C_SDA_0(void)
{
	uint8 ret = 0;
	do{
		Ql_GPIO_SetDirection(I2C_SDA_PIN,PINDIRECTION_OUT);
		Ql_GPIO_SetLevel(I2C_SDA_PIN,0);
	}while(0);
	return ret;
}
/*================================================================
【名 称】void I2C_Delay (uint8 t)
【功 能】模拟IIC用的短延时
================================================================*/
static void I2C_Delay (uint8 t)
{
	//osSleep(1);
}
/*================================================================
【名 称】void I2CInit(void)
【功 能】I2C初始化，空闲状态
================================================================*/
void Hal_I2C_Init(void)
{
	Ql_GPIO_Init(I2C_SCL_PIN,//Enum_PinName       pinName,
				PINDIRECTION_OUT,//Enum_PinDirection  dir,
				PINLEVEL_HIGH,//Enum_PinLevel      level,
				PINPULLSEL_DISABLE//Enum_PinPullSel    pullSel
	            );
	Ql_GPIO_Init(I2C_SDA_PIN,//Enum_PinName       pinName,
				PINDIRECTION_OUT,//Enum_PinDirection  dir,
				PINLEVEL_HIGH,//Enum_PinLevel      level,
				PINPULLSEL_PULLUP//Enum_PinPullSel    pullSel
	            );
	I2C_SDA_1();
	I2C_Delay(10);
	I2C_SCL_1();
	I2C_Delay(10);
}
/*================================================================
【名 称】void I2CStart(void)
【功 能】I2C启动信号
【备 注】SCL、SDA同为高，SDA跳变成低之后，SCL跳变成低
================================================================*/
void Hal_I2C_Start(void)
{
  	I2C_SDA_1();
  	I2C_SCL_1();
  	I2C_Delay(10);
  	I2C_SDA_0();
  	I2C_Delay(10);
  	I2C_SCL_0();
  	I2C_Delay(10);
}
/*================================================================
【名 称】void I2CStop(void)
【功 能】I2C停止信号
【备 注】SCL、SDA同为低，SCL跳变成高之后，SDA跳变成高
================================================================*/
void Hal_I2C_Stop(void)
{
		I2C_SDA_0();
  	I2C_SCL_0();
		I2C_Delay(10);
  	I2C_SCL_1();
  	I2C_Delay(10);
  	I2C_SDA_1();
  	I2C_Delay(10);
}
/*================================================================
【名 称】uint8 I2CWRByte(uint8 WriteData)
【功 能】I2C写一个字节数据，返回ACK或者NACK
【备 注】从高到低，依次发送
================================================================*/
uint8 Hal_I2C_WriteByte(uint8 WriteData)
{
	uint8 i;
	I2C_SCL_0();
	for(i = 0;i < 8;i++)
	{
		if(WriteData & 0x80)
			{
			I2C_SDA_1();
			}
		else
			I2C_SDA_0();
		I2C_Delay(100);
		I2C_SCL_1();	    //输出SDA稳定后，拉高SCL给出上升沿，从机检测到后进行数据采样
		I2C_Delay(500);
		I2C_SCL_0();
		I2C_Delay(100);
		WriteData <<= 1;
//		I2C_SCL_0();
	}
	I2C_Delay(20);
	I2C_SDA_1();
	I2C_SCL_1();
	I2C_Delay(30);
	if(I2C_SDA_READ() == 1)			//SDA为高，收到NACK
	{
		return I2C_NACK;
	}
	else 				    //SDA为低，收到ACK
	{
		I2C_SCL_0();
		I2C_Delay(50);
		return I2C_ACK;
	}
}
/*================================================================
【名 称】uint8 I2CRDByte(uint8 AckValue)
【功 能】I2C读一个字节数据，入口参数用于控制应答状态，ACK或者NACK
【备 注】从高到低，依次接收
================================================================*/
uint8 Hal_I2C_ReadByte(uint8 AckValue)
{
	uint8 i,ReadData = 0;
	I2C_SCL_0();
	I2C_SDA_1();			//释放总线
	for(i = 0;i < 8;i++)
	{
		I2C_SCL_1();		//给出上升沿
		I2C_Delay(30);	//延时等待信号稳定
		ReadData <<= 1;
		if(I2C_SDA_READ() == 1) 		//采样获取数据
		{
			ReadData |= 0x01;
		}
		else
		{
			ReadData &= 0xfe;
		}
		I2C_Delay(10);
		I2C_SCL_0();   		//下降沿，从机给出下一位值
		I2C_Delay(20);
	}
	if(AckValue)//应答状态
	{
		I2C_SCL_1();
	}
	else
	{
		I2C_SCL_0();
	}
	I2C_Delay(10);
	I2C_SCL_1();
  	I2C_Delay(50);
  	I2C_SCL_0();
	  I2C_Delay(50);
	return ReadData;
}
