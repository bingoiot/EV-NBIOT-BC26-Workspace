#ifndef _W25QXX_H
#define _W25QXX_H

#include "hal_spi.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
typedef enum
{
	W25Q10=1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
	
}W25QXX_ID_t;

typedef struct
{
	W25QXX_ID_t		ID;
	uint8			UniqID[8];

	uint16    	PageSize;
	uint32		PageCount;
	uint32		SectorSize;
	uint32		SectorCount;
	uint32		BlockSize;
	uint32		BlockCount;

	uint32		CapacityInKiloByte;
	
	uint8			StatusRegister1;
	uint8			StatusRegister2;
	uint8			StatusRegister3;
	
	uint8			Lock;
	
}w25qxx_t;

extern w25qxx_t	w25qxx;
//############################################################################
// in Page,Sector and block read/write functions, can put 0 to read maximum bytes 
//############################################################################
osState	W25qxx_Init(void);

void 	W25qxx_BlockUnlock(void);
void 	W25qxx_BlockLock(void);
void 	W25qxx_WriteEnable(void);
void 	W25qxx_WriteDisable(void);
void 	W25qxx_WaitForWriteEnd(void);

void	W25qxx_EraseChip(void);
void 	W25qxx_EraseSector(uint32 SectorAddr);
void 	W25qxx_EraseBlock(uint32 BlockAddr);

uint32	W25qxx_PageToSector(uint32	PageAddress);
uint32	W25qxx_PageToBlock(uint32	PageAddress);
uint32	W25qxx_SectorToBlock(uint32	SectorAddress);
uint32	W25qxx_SectorToPage(uint32	SectorAddress);
uint32	W25qxx_BlockToPage(uint32	BlockAddress);

osState 	W25qxx_IsEmptyPage(uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_PageSize);
osState 	W25qxx_IsEmptySector(uint32 Sector_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_SectorSize);
osState 	W25qxx_IsEmptyBlock(uint32 Block_Address,uint32 OffsetInByte,uint32 NumByteToCheck_up_to_BlockSize);

void 	W25qxx_WriteByte(uint8 pBuffer,uint32 Bytes_Address);
void 	W25qxx_WriteBytes(uint8 *pBuffer, uint32 NumByteToWrite_up_to_PageSize,uint32 WriteAddr_inBytes);
void 	W25qxx_WritePage(uint8 *pBuffer	,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToWrite_up_to_PageSize);
void 	W25qxx_WriteSector(uint8 *pBuffer,uint32 Sector_Address,uint32 OffsetInByte,uint32 NumByteToWrite_up_to_SectorSize);
void 	W25qxx_WriteBlock(uint8* pBuffer,uint32 Block_Address,uint32 OffsetInByte,uint32 NumByteToWrite_up_to_BlockSize);

void 	W25qxx_ReadByte(uint8 *pBuffer,uint32 Bytes_Address);
void 	W25qxx_ReadBytes(uint8 *pBuffer,uint32 ReadAddr,uint32 NumByteToRead);
void 	W25qxx_ReadPage(uint8 *pBuffer,uint32 Page_Address,uint32 OffsetInByte,uint32 NumByteToRead_up_to_PageSize);
void 	W25qxx_ReadSector(uint8 *pBuffer,uint32 Sector_Address,uint32 OffsetInByte,uint32 NumByteToRead_up_to_SectorSize);
void 	W25qxx_ReadBlock(uint8* pBuffer,uint32 Block_Address,uint32 OffsetInByte,uint32	NumByteToRead_up_to_BlockSize);
//############################################################################
#ifdef __cplusplus
}
#endif

#endif

