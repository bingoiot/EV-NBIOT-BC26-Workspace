
#if (_W25QXX_DEBUG==1)
#include <stdio.h>
#endif
#include "w25qxx.h"
#include "w25qxxConfig.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_gpio.h"
#include "ql_spi.h"
#include "ql_error.h"
#include "hal_spi.h"

//#define W25QXX_DUMMY_BYTE         0xA5

#define DEBUG_W25QXX				0

w25qxx_t	w25qxx;

#if (_W25QXX_USE_FREERTOS==1)
#define	W25qxx_Delay(delay)		osSleep(delay)
#include "osCore.h"
#else
#define	W25qxx_Delay(delay)		HAL_Delay(delay)
#endif

uint8 W25qxx_ReadStatusRegister(uint8	SelectStatusRegister_1_2_3);
//###################################################################################################################
uint8	W25qxx_WriteReadOneByte(uint8	Data)
{
	uint8	ret;
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,&Data,1,&ret,1);
	return ret;	
}
uint8 W25qxx_WriteOneByte(uint8	Data)
{
	uint8	ret;
	ret = Ql_SPI_Write(USR_SPI_CHANNAL, &Data, 1);
	return ret;
}
void W25qxx_BlockUnlock(void)
{
    s32 ret = 0;
    u8 wr_buff[32]={0};

    wr_buff[0] = 0x01;
    W25qxx_ReadStatusRegister(1);
    W25qxx_ReadStatusRegister(2);
    if(w25qxx.StatusRegister2 & 0x40)
        wr_buff[1] = w25qxx.StatusRegister1 | 0x7C;
    else
        wr_buff[1] = w25qxx.StatusRegister1 & (~0x7C);
    W25qxx_WriteEnable();
    hal_spi_cs(0);
    ret = Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,2);
    hal_spi_cs(1);
    if(ret==2)
    {
        osLogI(DEBUG_W25QXX,"[Debug Log]flash unlock ok.res:%02x@:%02x\r\n",w25qxx.StatusRegister1,w25qxx.StatusRegister2);
    }
    else
    {
    	osLogI(DEBUG_W25QXX,"[Debug Log]flash unlock ok failed, ret=%d.res:%02x@:%02x\r\n",ret,w25qxx.StatusRegister1,w25qxx.StatusRegister2);
    }
}
void W25qxx_BlockLock(void)
{
    s32 ret = 0;
    u8 wr_buff[32]={0};

    wr_buff[0] = 0x01;
    wr_buff[1] = (w25qxx.StatusRegister1 & (~0x7C)) | 0x18;
    wr_buff[2] = w25qxx.StatusRegister2 | 0x40;

    W25qxx_WriteEnable();
    hal_spi_cs(0);
    ret = Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,3);
    hal_spi_cs(1);
    if(ret == 3)
    {
    	 osLogI(DEBUG_W25QXX,"[Debug Log]flash lock ok.\r\n");
    }
    else
    {
    	 osLogI(DEBUG_W25QXX,"[Debug Log]flash lock ok failed, ret=%d.\r\n", ret);
    }
}

//###################################################################################################################
uint32 W25qxx_ReadID(void)
{
    u8 wr_buff[32]={0x9f};
    u8 rd_buff[32]={0xff};
	uint32 Temp;
	hal_spi_cs(0);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,1,rd_buff,3);
	hal_spi_cs(1);
	Temp = rd_buff[0];
	Temp <<= 8;
	Temp |= rd_buff[1];
	Temp <<= 8;
	Temp |= rd_buff[2];
	osLogI(DEBUG_W25QXX,"ChipID:%08x \r\n",Temp);
	return Temp;
}
//###################################################################################################################
void W25qxx_ReadUniqID(void)
{
	u8 wr_buff[32]={0x4B,0xFF,0xFF,0xFF};
	hal_spi_cs(0);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,1,w25qxx.UniqID,8);
	hal_spi_cs(1);
	osLogB(DEBUG_W25QXX,"RedUniqID:",w25qxx.UniqID,8);
}
void W25qxx_ReadID2(void)
{
    s32 ret = 0;
    u8 wr_buff[32]={0x90,0xFF,0xFF,0x00};
    u8 rd_buff[32]={0xff};
	hal_spi_cs(0);
    ret = Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,4,rd_buff,2);
	hal_spi_cs(1);
    if(ret==2)
    {
    	osLogI(DEBUG_W25QXX,"[Debug Log]0x%x 0x%x\r\n", rd_buff[0],rd_buff[1]);
    }
    else
    {
    	osLogI(DEBUG_W25QXX,"[Debug Log]read flash id 2 failed, ret=%d", ret);
    }
    W25qxx_WriteDisable();
}
//###################################################################################################################
void W25qxx_WriteEnable(void)
{
	hal_spi_cs(0);
	W25qxx_WriteOneByte(0x06);
	hal_spi_cs(1);
	W25qxx_Delay(1);
	osLogI(DEBUG_W25QXX,"W25qxx_WriteEnable \r\n");
}
//###################################################################################################################
void W25qxx_WriteDisable(void)
{
	hal_spi_cs(0);
	W25qxx_WriteOneByte(0x04);
	hal_spi_cs(1);
	W25qxx_Delay(1);
	osLogI(DEBUG_W25QXX,"W25qxx_WriteDisable \r\n");
}
//###################################################################################################################
uint8 W25qxx_ReadStatusRegister(uint8	SelectStatusRegister_1_2_3)
{
	uint8	status=0;
	u8 wr_buff[32]={0x35};

	hal_spi_cs(0);
	if(SelectStatusRegister_1_2_3==1)
	{
		wr_buff[0] = 0x05;
		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,1,&status,1);
		w25qxx.StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3==2)
	{
		wr_buff[0] = 0x35;
		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,1,&status,1);
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		wr_buff[0] = 0x15;
		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,1,&status,1);
		w25qxx.StatusRegister3 = status;
	}	
	hal_spi_cs(1);
	return status;
}
//###################################################################################################################
void W25qxx_WriteStatusRegister(uint8	SelectStatusRegister_1_2_3,uint8 Data)
{
    u8 wr_buff[32];
    W25qxx_WaitForWriteEnd();
    W25qxx_WriteEnable();
	hal_spi_cs(0);
	if(SelectStatusRegister_1_2_3==1)
	{
		wr_buff[0] = (0x01);
		w25qxx.StatusRegister1 = Data;
	}
	else if(SelectStatusRegister_1_2_3==2)
	{
		wr_buff[0] = (0x31);
		w25qxx.StatusRegister2 = Data;
	}
	else
	{
		wr_buff[0] = (0x11);
		w25qxx.StatusRegister3 = Data;
	}
	wr_buff[1] = Data;
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,2);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
}
//###################################################################################################################
void W25qxx_WaitForWriteEnd(void)
{
	W25qxx_Delay(1);
	hal_spi_cs(0);
	do
	{
		w25qxx.StatusRegister1 = W25qxx_WriteReadOneByte(0x05);
		W25qxx_Delay(1);
	}
	while ((w25qxx.StatusRegister1 & 0x01) == 0x01);
	hal_spi_cs(1);
}
//###################################################################################################################
osState	W25qxx_Init(void)
{
	uint32	id;
	hal_spi_init();
	w25qxx.Lock=1;	
	osLogI(_W25QXX_DEBUG,"w25qxx Init Begin...\r\n");
	id=W25qxx_ReadID();
	osLogI(_W25QXX_DEBUG,"w25qxx ID:0x%X\r\n",id);
	switch(id&0x0000FFFF)
	{
		case 0x401A:	// 	w25q512
			w25qxx.ID=W25Q512;
			w25qxx.BlockCount=1024;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q512\r\n");
		break;
		case 0x4019:	// 	w25q256
			w25qxx.ID=W25Q256;
			w25qxx.BlockCount=512;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q256\r\n");
		break;
		case 0x4018:	// 	w25q128
			w25qxx.ID=W25Q128;
			w25qxx.BlockCount=256;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q128\r\n");
		break;
		case 0x4017:	//	w25q64
			w25qxx.ID=W25Q64;
			w25qxx.BlockCount=128;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q64\r\n");
		break;
		case 0x4016:	//	w25q32
			w25qxx.ID=W25Q32;
			w25qxx.BlockCount=64;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q32\r\n");
		break;
		case 0x4015:	//	w25q16
			w25qxx.ID=W25Q16;
			w25qxx.BlockCount=32;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q16\r\n");
		break;
		case 0x4014:	//	w25q80
			w25qxx.ID=W25Q80;
			w25qxx.BlockCount=16;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q80\r\n");
		break;
		case 0x4013:	//	w25q40
			w25qxx.ID=W25Q40;
			w25qxx.BlockCount=8;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q40\r\n");
		break;
		case 0x4012:	//	w25q20
			w25qxx.ID=W25Q20;
			w25qxx.BlockCount=4;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q20\r\n");
		break;
		case 0x4011:	//	w25q10
			w25qxx.ID=W25Q10;
			w25qxx.BlockCount=2;
			osLogI(_W25QXX_DEBUG,"w25qxx Chip: w25q10\r\n");
		break;
		default:
			osLogI(_W25QXX_DEBUG,"w25qxx Unknown ID\r\n");
			w25qxx.Lock=0;	
			return osFalse;
				
	}		
	w25qxx.PageSize=256;
	w25qxx.SectorSize=0x1000;
	w25qxx.SectorCount=w25qxx.BlockCount*16;
	w25qxx.PageCount=(w25qxx.SectorCount*w25qxx.SectorSize)/w25qxx.PageSize;
	w25qxx.BlockSize=w25qxx.SectorSize*16;
	w25qxx.CapacityInKiloByte=(w25qxx.SectorCount*w25qxx.SectorSize)/1024;
	W25qxx_ReadUniqID();
	W25qxx_ReadStatusRegister(1);
	W25qxx_ReadStatusRegister(2);
	W25qxx_ReadStatusRegister(3);
	osLogI(_W25QXX_DEBUG,"w25qxx Page Size: %d Bytes\r\n",w25qxx.PageSize);
	osLogI(_W25QXX_DEBUG,"w25qxx Page Count: %d\r\n",w25qxx.PageCount);
	osLogI(_W25QXX_DEBUG,"w25qxx Sector Size: %d Bytes\r\n",w25qxx.SectorSize);
	osLogI(_W25QXX_DEBUG,"w25qxx Sector Count: %d\r\n",w25qxx.SectorCount);
	osLogI(_W25QXX_DEBUG,"w25qxx Block Size: %d Bytes\r\n",w25qxx.BlockSize);
	osLogI(_W25QXX_DEBUG,"w25qxx Block Count: %d\r\n",w25qxx.BlockCount);
	osLogI(_W25QXX_DEBUG,"w25qxx Capacity: %d KiloBytes\r\n",w25qxx.CapacityInKiloByte);
	osLogI(_W25QXX_DEBUG,"w25qxx Init Done\r\n");
	w25qxx.Lock=0;	
	return osTrue;
}	
//###################################################################################################################
void	W25qxx_EraseChip(void)
{
	while(w25qxx.Lock==1)
	W25qxx_Delay(1);
	w25qxx.Lock=1;	
	uint32	StartTime=osGetmsTimestamp();
	osLogI(_W25QXX_DEBUG,"w25qxx EraseChip Begin...\r\n");
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	W25qxx_WriteReadOneByte(0xC7);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	osLogI(_W25QXX_DEBUG,"w25qxx EraseChip done after %d ms!\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(10);
	w25qxx.Lock=0;	
}
//###################################################################################################################
void W25qxx_EraseSector(uint32 SectorAddr)
{
	int j;
	uint8 wr_buff[4];

	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;	
	#if (_W25QXX_DEBUG==1)
	uint32	StartTime=osGetmsTimestamp();
	osLogI(_W25QXX_DEBUG,"w25qxx EraseSector %d Begin...\r\n",SectorAddr);
	#endif
	SectorAddr = SectorAddr * w25qxx.SectorSize;
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = (0x20);
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((SectorAddr & 0xFF000000) >> 24);
	wr_buff[j++] = ((SectorAddr & 0xFF0000) >> 16);
	wr_buff[j++] = ((SectorAddr & 0xFF00) >> 8);
	wr_buff[j++] = (SectorAddr & 0xFF);
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,j);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx EraseSector done after %d ms\r\n",osGetmsTimestamp()-StartTime);
	#endif
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}
//###################################################################################################################
void W25qxx_EraseBlock(uint32 BlockAddr)
{
	int j;
	uint8 wr_buff[4];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;	
	osLogI(_W25QXX_DEBUG,"w25qxx EraseBlock %d Begin...\r\n",BlockAddr);
	uint32	StartTime=osGetmsTimestamp();
	BlockAddr = BlockAddr * w25qxx.SectorSize*16;
	j = 0;
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	wr_buff[j++] = 0xD8;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((BlockAddr & 0xFF000000) >> 24);
	wr_buff[j++] = ((BlockAddr & 0xFF0000) >> 16);
	wr_buff[j++] = ((BlockAddr & 0xFF00) >> 8);
	wr_buff[j++] = (BlockAddr & 0xFF);
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,j);
  	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	osLogI(_W25QXX_DEBUG,"w25qxx EraseBlock done after %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}
//###################################################################################################################
uint32	W25qxx_PageToSector(uint32	PageAddress)
{
	return ((PageAddress*w25qxx.PageSize)/w25qxx.SectorSize);
}
//###################################################################################################################
uint32	W25qxx_PageToBlock(uint32	PageAddress)
{
	return ((PageAddress*w25qxx.PageSize)/w25qxx.BlockSize);
}
//###################################################################################################################
uint32	W25qxx_SectorToBlock(uint32	SectorAddress)
{
	return ((SectorAddress*w25qxx.SectorSize)/w25qxx.BlockSize);
}
//###################################################################################################################
uint32	W25qxx_SectorToPage(uint32	SectorAddress)
{
	return (SectorAddress*w25qxx.SectorSize)/w25qxx.PageSize;
}
//###################################################################################################################
uint32	W25qxx_BlockToPage(uint32	BlockAddress)
{
	return (BlockAddress*w25qxx.BlockSize)/w25qxx.PageSize;
}
//###################################################################################################################
osState 	W25qxx_IsEmptyPage(uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_PageSize)
{
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
	W25qxx_Delay(1);
	w25qxx.Lock=1;	
	if(((NumByteToCheck_up_to_PageSize+OffsetInByte)>w25qxx.PageSize)||(NumByteToCheck_up_to_PageSize==0))
		NumByteToCheck_up_to_PageSize=w25qxx.PageSize-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckPage:%d, Offset:%d, Bytes:%d begin...\r\n",Page_Address,OffsetInByte,NumByteToCheck_up_to_PageSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif		
	uint8		pBuffer[32];
	uint32	WorkAddress;
	uint32	i,j;
	for(i=OffsetInByte; i<w25qxx.PageSize; i+=sizeof(pBuffer))
	{
		j = 0;
		hal_spi_cs(0);
		WorkAddress=(i+Page_Address*w25qxx.PageSize);
		wr_buff[j++] = 0x0B;
		if(w25qxx.ID>=W25Q256)
			wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
		wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
		wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
		wr_buff[j++] = (WorkAddress & 0xFF);
		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
		hal_spi_cs(1);
		for(uint8 x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((w25qxx.PageSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<w25qxx.PageSize; i++)
		{
			j = 0;
			hal_spi_cs(0);
			WorkAddress=(i+Page_Address*w25qxx.PageSize);
			wr_buff[j++] = 0x0B;
			if(w25qxx.ID>=W25Q256)
				wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
			wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
			wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
			wr_buff[j++] = (WorkAddress & 0xFF);
			Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
			hal_spi_cs(1);
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckPage is Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osTrue;
	NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckPage is Not Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osFalse;
}
//###################################################################################################################
osState 	W25qxx_IsEmptySector(uint32 Sector_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_SectorSize)
{
	int j;
	uint8 wr_buff[32];

	while(w25qxx.Lock==1)
	W25qxx_Delay(1);
	w25qxx.Lock=1;	
	if((NumByteToCheck_up_to_SectorSize>w25qxx.SectorSize)||(NumByteToCheck_up_to_SectorSize==0))
		NumByteToCheck_up_to_SectorSize=w25qxx.SectorSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckSector:%d, Offset:%d, Bytes:%d begin...\r\n",Sector_Address,OffsetInByte,NumByteToCheck_up_to_SectorSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif		
	uint8	pBuffer[32];
	uint32	WorkAddress;
	uint32	i;
	for(i=OffsetInByte; i<w25qxx.SectorSize; i+=sizeof(pBuffer))
	{
		j = 0;
		hal_spi_cs(0);
		WorkAddress=(i+Sector_Address*w25qxx.SectorSize);
		wr_buff[j++] = 0x0B;
		if(w25qxx.ID>=W25Q256)
			wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
		wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
		wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
		wr_buff[j++] = (WorkAddress & 0xFF);

		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
		hal_spi_cs(1);;
		for(uint8 x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((w25qxx.SectorSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<w25qxx.SectorSize; i++)
		{
			j = 0;
			hal_spi_cs(0);
			WorkAddress=(i+Sector_Address*w25qxx.SectorSize);
			wr_buff[j++] = 0x0B;
			if(w25qxx.ID>=W25Q256)
				wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
			wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
			wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
			wr_buff[j++] = (WorkAddress & 0xFF);
			Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
			hal_spi_cs(1);
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckSector is Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osTrue;
	NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckSector is Not Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osFalse;
}
//###################################################################################################################
osState 	W25qxx_IsEmptyBlock(uint32 Block_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_BlockSize)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
	W25qxx_Delay(1);
	w25qxx.Lock=1;	
	if((NumByteToCheck_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToCheck_up_to_BlockSize==0))
		NumByteToCheck_up_to_BlockSize=w25qxx.BlockSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckBlock:%d, Offset:%d, Bytes:%d begin...\r\n",Block_Address,OffsetInByte,NumByteToCheck_up_to_BlockSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif		
	uint8	pBuffer[32];
	uint32	WorkAddress;
	uint32	i;
	for(i=OffsetInByte; i<w25qxx.BlockSize; i+=sizeof(pBuffer))
	{
		j = 0;
		hal_spi_cs(0);
		WorkAddress=(i+Block_Address*w25qxx.SectorSize);
		wr_buff[j++] = 0x0B;
		if(w25qxx.ID>=W25Q256)
			wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
		wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
		wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
		wr_buff[j++] = (WorkAddress & 0xFF);
		Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
		hal_spi_cs(1);
		for(uint8 x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((w25qxx.BlockSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<w25qxx.BlockSize; i++)
		{
			j = 0;
			hal_spi_cs(0);
			WorkAddress=(i+Block_Address*w25qxx.SectorSize);
			wr_buff[j++] = 0x0B;
			if(w25qxx.ID>=W25Q256)
				wr_buff[j++] = ((WorkAddress & 0xFF000000) >> 24);
			wr_buff[j++] = ((WorkAddress & 0xFF0000) >> 16);
			wr_buff[j++] = ((WorkAddress & 0xFF00) >> 8);
			wr_buff[j++] = (WorkAddress & 0xFF);
			Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j, pBuffer,sizeof(pBuffer));
			hal_spi_cs(1);
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckBlock is Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osTrue;
	NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx CheckBlock is Not Empty in %d ms\r\n",osGetmsTimestamp()-StartTime);
	W25qxx_Delay(100);
	#endif	
	w25qxx.Lock=0;
	return osFalse;
}
//###################################################################################################################
void W25qxx_WriteByte(uint8 pBuffer, uint32 WriteAddr_inBytes)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32	StartTime=osGetmsTimestamp();
	osLogI(_W25QXX_DEBUG,"w25qxx WriteByte 0x%02X at address %d begin...",pBuffer,WriteAddr_inBytes);
	#endif
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = 0x02;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((WriteAddr_inBytes & 0xFF000000) >> 24);
	wr_buff[j++] = ((WriteAddr_inBytes & 0xFF0000) >> 16);
	wr_buff[j++] = ((WriteAddr_inBytes & 0xFF00) >> 8);
	wr_buff[j++] = (WriteAddr_inBytes & 0xFF);
	wr_buff[j++] = pBuffer;
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,j);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx WriteByte done after %d ms\r\n",osGetmsTimestamp()-StartTime);
	#endif
	w25qxx.Lock=0;
}
//###################################################################################################################
void 	W25qxx_WritePage(uint8 *pBuffer	,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToWrite_up_to_PageSize)
{
	int j;
	uint8 wr_buff[512];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	#if (_W25QXX_DEBUG==1)
	osLogI(1,"w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToWrite_up_to_PageSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif	
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	osLogI(_W25QXX_DEBUG,"w25qxx WritePage: real addr:%d£¬ length:%d \r\n",Page_Address,NumByteToWrite_up_to_PageSize);
	j = 0;
	wr_buff[j++] = 0x02;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((Page_Address>>24) & 0xFF);
	wr_buff[j++] = ((Page_Address>>16) & 0xFF);
	wr_buff[j++] = ((Page_Address>>8) & 0xFF);
	wr_buff[j++] = ((Page_Address) & 0xFF);
	osMemcpy(&wr_buff[j],pBuffer,NumByteToWrite_up_to_PageSize);
	j+=NumByteToWrite_up_to_PageSize;
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,j);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	#if (_W25QXX_DEBUG==1)
	StartTime = osGetmsTimestamp()-StartTime;
	osLogB(1,"write:",wr_buff,j);
	osLogI(1,"w25qxx WritePage done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}
/*
void 	W25qxx_WritePage(uint8 *pBuffer	,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToWrite_up_to_PageSize)
{
	int j;
	uint8 wr_buff[512];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	if(((NumByteToWrite_up_to_PageSize+OffsetInByte)>w25qxx.PageSize)||(NumByteToWrite_up_to_PageSize==0))
		NumByteToWrite_up_to_PageSize=w25qxx.PageSize-OffsetInByte;
	if((OffsetInByte+NumByteToWrite_up_to_PageSize) > w25qxx.PageSize)
		NumByteToWrite_up_to_PageSize = w25qxx.PageSize-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToWrite_up_to_PageSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	hal_spi_cs(0);
	Page_Address = (Page_Address*w25qxx.PageSize)+OffsetInByte;
	osLogI(_W25QXX_DEBUG,"w25qxx WritePage: real addr:%d£¬ length:%d \r\n",Page_Address,NumByteToWrite_up_to_PageSize);
	j = 0;
	wr_buff[j++] = 0x02;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((Page_Address>>24) & 0xFF);
	wr_buff[j++] = ((Page_Address>>16) & 0xFF);
	wr_buff[j++] = ((Page_Address>>8) & 0xFF);
	wr_buff[j++] = ((Page_Address) & 0xFF);
	osMemcpy(&wr_buff[j],pBuffer,NumByteToWrite_up_to_PageSize);
	j+=NumByteToWrite_up_to_PageSize;
	Ql_SPI_Write(USR_SPI_CHANNAL,wr_buff,j);
	hal_spi_cs(1);
	W25qxx_WaitForWriteEnd();
	#if (_W25QXX_DEBUG==1)
	StartTime = osGetmsTimestamp()-StartTime;
	osLogB(_W25QXX_DEBUG,"write:",wr_buff,j);
	osLogI(_W25QXX_DEBUG,"w25qxx WritePage done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}*/
//###################################################################################################################
void 	W25qxx_WriteSector(uint8 *pBuffer	,uint32 Sector_Address,uint32 OffsetInByte	,uint32 NumByteToWrite_up_to_SectorSize)
{
	if((NumByteToWrite_up_to_SectorSize>w25qxx.SectorSize)||(NumByteToWrite_up_to_SectorSize==0))
		NumByteToWrite_up_to_SectorSize=w25qxx.SectorSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"+++w25qxx WriteSector:%d, Offset:%d ,Write %d Bytes, begin...\r\n",Sector_Address,OffsetInByte,NumByteToWrite_up_to_SectorSize);
	W25qxx_Delay(100);
	#endif	
	if(OffsetInByte>=w25qxx.SectorSize)
	{
		#if (_W25QXX_DEBUG==1)
		osLogI(_W25QXX_DEBUG,"---w25qxx WriteSector Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return;
	}	
	uint32	StartPage;
	int		BytesToWrite;
	uint32	LocalOffset;
	uint32	temp;
	if((OffsetInByte+NumByteToWrite_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToWrite = w25qxx.SectorSize-OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;	
	StartPage = (Sector_Address*w25qxx.SectorSize)+(OffsetInByte);
	LocalOffset = StartPage%w25qxx.PageSize;
	temp = 0;
	do
	{
		if(LocalOffset>0)
		{
			temp = w25qxx.PageSize-LocalOffset;
		}
		else if(BytesToWrite>w25qxx.PageSize)
		{
			temp = w25qxx.PageSize;
		}
		else
		{
			temp = BytesToWrite;
		}
		W25qxx_WritePage(pBuffer,(StartPage-LocalOffset),LocalOffset,temp);
		StartPage+=w25qxx.PageSize;
		BytesToWrite -= temp;
		pBuffer += temp;
		LocalOffset=0;
	}while(BytesToWrite>0);		
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"---w25qxx WriteSector Done\r\n");
	W25qxx_Delay(100);
	#endif	
}
//###################################################################################################################
void 	W25qxx_WriteBlock	(uint8* pBuffer ,uint32 Block_Address	,uint32 OffsetInByte	,uint32	NumByteToWrite_up_to_BlockSize)
{
	if((NumByteToWrite_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToWrite_up_to_BlockSize==0))
		NumByteToWrite_up_to_BlockSize=w25qxx.BlockSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"+++w25qxx WriteBlock:%d, Offset:%d ,Write %d Bytes, begin...\r\n",Block_Address,OffsetInByte,NumByteToWrite_up_to_BlockSize);
	W25qxx_Delay(100);
	#endif	
	if(OffsetInByte>=w25qxx.BlockSize)
	{
		#if (_W25QXX_DEBUG==1)
		osLogI(_W25QXX_DEBUG,"---w25qxx WriteBlock Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return;
	}	
	uint32	StartPage;
	int		BytesToWrite;
	uint32	LocalOffset;
	if((OffsetInByte+NumByteToWrite_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToWrite = w25qxx.BlockSize-OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;	
	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/w25qxx.PageSize);
	LocalOffset = OffsetInByte%w25qxx.PageSize;	
	do
	{		
		W25qxx_WritePage(pBuffer,StartPage,LocalOffset,BytesToWrite);
		StartPage++;
		BytesToWrite-=w25qxx.PageSize-LocalOffset;
		pBuffer+=w25qxx.PageSize;	
		LocalOffset=0;
	}while(BytesToWrite>0);		
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"---w25qxx WriteBlock Done\r\n");
	W25qxx_Delay(100);
	#endif	
}
//###################################################################################################################
void 	W25qxx_ReadByte(uint8 *pBuffer,uint32 Bytes_Address)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32	StartTime=osGetmsTimestamp();
	osLogI(_W25QXX_DEBUG,"w25qxx ReadByte at address %d begin...\r\n",Bytes_Address);
	#endif
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = 0x0B;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((Bytes_Address & 0xFF000000) >> 24);
	wr_buff[j++] = ((Bytes_Address & 0xFF0000) >> 16);
	wr_buff[j++] = ((Bytes_Address & 0xFF00) >> 8);
	wr_buff[j++] = (Bytes_Address & 0xFF);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j,pBuffer,1);
	hal_spi_cs(1);
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx ReadByte 0x%02X done after %d ms\r\n",*pBuffer,osGetmsTimestamp()-StartTime);
	#endif
	w25qxx.Lock=0;
}
//###################################################################################################################
void W25qxx_ReadBytes(uint8* pBuffer, uint32 ReadAddr, uint32 NumByteToRead)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32	StartTime=osGetmsTimestamp();
	osLogI(_W25QXX_DEBUG,"w25qxx ReadBytes at Address:%d, %d Bytes  begin...\r\n",ReadAddr,NumByteToRead);
	#endif	
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = 0x0B;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((ReadAddr & 0xFF000000) >> 24);
	wr_buff[j++] = ((ReadAddr & 0xFF0000) >> 16);
	wr_buff[j++] = ((ReadAddr & 0xFF00) >> 8);
	wr_buff[j++] = (ReadAddr & 0xFF);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j,pBuffer,NumByteToRead);
	hal_spi_cs(1);
	#if (_W25QXX_DEBUG==1)
	StartTime = osGetmsTimestamp()-StartTime;
	osLogB(_W25QXX_DEBUG,NULL,pBuffer,NumByteToRead);
	osLogI(_W25QXX_DEBUG,"w25qxx ReadBytes done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}
//###################################################################################################################
void 	W25qxx_ReadPage(uint8 *pBuffer,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToRead_up_to_PageSize)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	#if (_W25QXX_DEBUG==1)
	osLogI(1,"w25qxx ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToRead_up_to_PageSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif	
	osLogI(_W25QXX_DEBUG,"w25qxx ReadPage: real addr:%d\r\n",Page_Address);
	W25qxx_WaitForWriteEnd();
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = 0x03;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((Page_Address & 0xFF000000) >> 24);
	wr_buff[j++] = ((Page_Address & 0xFF0000) >> 16);
	wr_buff[j++] = ((Page_Address & 0xFF00) >> 8);
	wr_buff[j++] = (Page_Address & 0xFF);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j,pBuffer,NumByteToRead_up_to_PageSize);
	hal_spi_cs(1);
	#if (_W25QXX_DEBUG==1)
	StartTime = osGetmsTimestamp()-StartTime;
	osLogB(1,NULL,pBuffer,NumByteToRead_up_to_PageSize);
	osLogI(1,"w25qxx ReadPage done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}
/*
void 	W25qxx_ReadPage(uint8 *pBuffer,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToRead_up_to_PageSize)
{
	int j;
	uint8 wr_buff[32];
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);
	w25qxx.Lock=1;
	if((NumByteToRead_up_to_PageSize>w25qxx.PageSize)||(NumByteToRead_up_to_PageSize==0))
		NumByteToRead_up_to_PageSize=w25qxx.PageSize;
	if((OffsetInByte+NumByteToRead_up_to_PageSize) > w25qxx.PageSize)
		NumByteToRead_up_to_PageSize = w25qxx.PageSize-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"w25qxx ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToRead_up_to_PageSize);
	W25qxx_Delay(100);
	uint32	StartTime=osGetmsTimestamp();
	#endif
	Page_Address = Page_Address*w25qxx.PageSize+OffsetInByte;
	osLogI(_W25QXX_DEBUG,"w25qxx ReadPage: real addr:%d\r\n",Page_Address);
	hal_spi_cs(0);
	j = 0;
	wr_buff[j++] = 0x03;
	if(w25qxx.ID>=W25Q256)
		wr_buff[j++] = ((Page_Address & 0xFF000000) >> 24);
	wr_buff[j++] = ((Page_Address & 0xFF0000) >> 16);
	wr_buff[j++] = ((Page_Address & 0xFF00) >> 8);
	wr_buff[j++] = (Page_Address & 0xFF);
	Ql_SPI_WriteRead(USR_SPI_CHANNAL,wr_buff,j,pBuffer,NumByteToRead_up_to_PageSize);
	hal_spi_cs(1);
	#if (_W25QXX_DEBUG==1)
	StartTime = osGetmsTimestamp()-StartTime;
	osLogB(_W25QXX_DEBUG,NULL,pBuffer,NumByteToRead_up_to_PageSize);
	osLogI(_W25QXX_DEBUG,"w25qxx ReadPage done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif
	W25qxx_Delay(1);
	w25qxx.Lock=0;
}*/
//###################################################################################################################
void 	W25qxx_ReadSector(uint8 *pBuffer,uint32 Sector_Address,uint32 OffsetInByte,uint32 NumByteToRead_up_to_SectorSize)
{	
	if((NumByteToRead_up_to_SectorSize>w25qxx.SectorSize)||(NumByteToRead_up_to_SectorSize==0))
		NumByteToRead_up_to_SectorSize=w25qxx.SectorSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"+++w25qxx ReadSector:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Sector_Address,OffsetInByte,NumByteToRead_up_to_SectorSize);
	W25qxx_Delay(100);
	#endif	
	if(OffsetInByte>=w25qxx.SectorSize)
	{
		#if (_W25QXX_DEBUG==1)
		osLogI(_W25QXX_DEBUG,"---w25qxx ReadSector Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return;
	}	
	uint32	StartPage;
	int		BytesToRead;
	uint32	LocalOffset;
	uint32	temp;
	if((OffsetInByte+NumByteToRead_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToRead = w25qxx.SectorSize-OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;	
	StartPage = (Sector_Address*w25qxx.SectorSize)+(OffsetInByte);
	LocalOffset = StartPage%w25qxx.PageSize;
	do
	{
		if(LocalOffset>0)
		{
			temp = w25qxx.PageSize-LocalOffset;
		}
		else if(BytesToRead>w25qxx.PageSize)
		{
			temp = w25qxx.PageSize;
		}
		else
		{
			temp = BytesToRead;
		}
		W25qxx_ReadPage(pBuffer,(StartPage-LocalOffset),LocalOffset,temp);
		StartPage+=w25qxx.PageSize;
		BytesToRead-=temp;
		pBuffer+=temp;
		LocalOffset=0;

	}while(BytesToRead>0);		
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"---w25qxx ReadSector Done\r\n");
	W25qxx_Delay(100);
	#endif	
}
//###################################################################################################################
void 	W25qxx_ReadBlock(uint8* pBuffer,uint32 Block_Address,uint32 OffsetInByte,uint32	NumByteToRead_up_to_BlockSize)
{
	if((NumByteToRead_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToRead_up_to_BlockSize==0))
		NumByteToRead_up_to_BlockSize=w25qxx.BlockSize;
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"+++w25qxx ReadBlock:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Block_Address,OffsetInByte,NumByteToRead_up_to_BlockSize);
	W25qxx_Delay(100);
	#endif	
	if(OffsetInByte>=w25qxx.BlockSize)
	{
		#if (_W25QXX_DEBUG==1)
		osLogI(_W25QXX_DEBUG,"w25qxx ReadBlock Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return;
	}	
	uint32	StartPage;
	int		BytesToRead;
	uint32	LocalOffset;
	if((OffsetInByte+NumByteToRead_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToRead = w25qxx.BlockSize-OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_BlockSize;	
	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/w25qxx.PageSize);
	LocalOffset = OffsetInByte%w25qxx.PageSize;	
	do
	{		
		W25qxx_ReadPage(pBuffer,StartPage,LocalOffset,BytesToRead);
		StartPage++;
		BytesToRead-=w25qxx.PageSize-LocalOffset;
		pBuffer+=w25qxx.PageSize;	
		LocalOffset=0;
	}while(BytesToRead>0);		
	#if (_W25QXX_DEBUG==1)
	osLogI(_W25QXX_DEBUG,"---w25qxx ReadBlock Done\r\n");
	W25qxx_Delay(100);
	#endif	
}
//###################################################################################################################

