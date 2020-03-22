/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <quackmore-ff@yahoo.com> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy me a beer in return. Quackmore
 * ----------------------------------------------------------------------------
 */

#include "osCore.h"
#include "w25qxx.h"
#include "spiffs_flash_functions.h"
#include "fs_entry.h"

#define DBG_HAL_SPI		0
// flash read function (checkout SPIFFS documentation)
s32_t  esp_spiffs_read(u32_t t_addr, u32_t t_size, u8_t *t_dst)
{
	uint8 *pbuf;
	uint32 offset,start_addr,index,slen;
    osLogI(DBG_HAL_SPI,"esp_spiffs_read:addr:%08x, size:%d \r\n",t_addr,t_size);
    // boundary checks
    if ((t_addr < SPIFFS_START_ADDR) || (t_addr >= SPIFFS_END_ADDR) ||
        (t_addr + ((t_size / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) > SPIFFS_END_ADDR))
    {
        P_ERROR("[ERROR]: Flash file system boundary error!\n");
        P_ERROR("[ERROR]: READ to address: %X, size: %d\n", t_addr, t_size);
        return SPIFFS_FLASH_BOUNDARY_ERROR;
    }
    // let's use aligned ram variable
    // warning: using stack instead of heap will produce hallucinations
    if((pbuf = osMalloc(SPIFFS_LOG_PAGE_SIZE + FS_ALIGN_BYTES))!=NULL)
    {
    	offset = t_addr % SPIFFS_LOG_PAGE_SIZE;
    	start_addr = (t_addr/SPIFFS_LOG_PAGE_SIZE)*SPIFFS_LOG_PAGE_SIZE;
    	index = 0;
    	while(index < t_size)
    	{
    		if(offset)
    		{
    			slen = (SPIFFS_LOG_PAGE_SIZE - offset);
    			if(slen > t_size)
    				slen = t_size;
    		}
    		else
    		{
				slen = t_size - index;
				if(slen > SPIFFS_LOG_PAGE_SIZE)
					slen = SPIFFS_LOG_PAGE_SIZE;
    		}
    		osLogI(DBG_HAL_SPI,"W25qxx_ReadPage:addr:%08x, offset:%d size:%d \r\n",start_addr,offset,slen);
    		W25qxx_ReadPage((uint8 *)pbuf,start_addr, 0, SPIFFS_LOG_PAGE_SIZE);
    		osMemcpy(&t_dst[index],&pbuf[offset],slen);
    		index += slen;
    		start_addr += slen;
    		offset = 0;
    	}
    	osFree((void *)pbuf);
    }
    return SPIFFS_OK;
}

// flash write function (checkout SPIFFS documentation)
s32_t  esp_spiffs_write(u32_t t_addr, u32_t t_size, u8_t *t_src)
{
	uint8 *pbuf;
	uint32 offset,start_addr,index,slen;
    osLogI(DBG_HAL_SPI,"esp_spiffs_write:addr:%08x, size:%d \r\n",t_addr,t_size);
    // boundary checks
    if ((t_addr < SPIFFS_START_ADDR) || (t_addr >= SPIFFS_END_ADDR) ||
        (t_addr + ((t_size / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) > SPIFFS_END_ADDR))
    {
        P_ERROR("[ERROR]: Flash file system boundary error!\n");
        P_ERROR("[ERROR]: Writing to address: %X, size: %d\n", t_addr, t_size);
        return SPIFFS_FLASH_BOUNDARY_ERROR;
    }
    // let's use aligned ram variable
    // warning: using stack instead of heap will produce hallucinations
    if((pbuf = osMalloc(SPIFFS_LOG_PAGE_SIZE + FS_ALIGN_BYTES))!=NULL)
    {
    	offset = t_addr % SPIFFS_LOG_PAGE_SIZE;
    	start_addr = (t_addr/SPIFFS_LOG_PAGE_SIZE)*SPIFFS_LOG_PAGE_SIZE;
    	index = 0;
    	while(index < t_size)
    	{
    		if(offset)
    		{
    			slen = (SPIFFS_LOG_PAGE_SIZE - offset);
    			if(slen > t_size)
    				slen = t_size;
    		}
    		else
    		{
				slen = t_size - index;
				if(slen > SPIFFS_LOG_PAGE_SIZE)
					slen = SPIFFS_LOG_PAGE_SIZE;
    		}
    		osLogI(DBG_HAL_SPI,"W25qxx_WritePage:addr:%08x, offset:%d size:%d \r\n",start_addr,offset,slen);
    		W25qxx_ReadPage((uint8 *)pbuf,start_addr, 0, SPIFFS_LOG_PAGE_SIZE);
    		osMemcpy(&pbuf[offset],&t_src[index],slen);
    		W25qxx_WritePage((uint8 *)pbuf, start_addr, 0,SPIFFS_LOG_PAGE_SIZE);
    		index += slen;
    		start_addr += slen;
    		offset = 0;
    	}
    	osFree((void *)pbuf);
    }
    return SPIFFS_OK;
}

// flash erase function (checkout SPIFFS documentation)
s32_t  esp_spiffs_erase(u32_t t_addr, u32_t t_size)
{
    // P_TRACE("[TRACE]: spiffs erase called ------------------------------------\n");
    // boundary checks
    if ((((t_addr / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) < SPIFFS_START_ADDR) ||
        (((t_addr / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) >= SPIFFS_END_ADDR) ||
        (((t_addr / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) + ((t_size / FS_ALIGN_BYTES) * FS_ALIGN_BYTES) > SPIFFS_END_ADDR))
    {
        P_ERROR("[ERROR]: Flash file system boundary error!\n");
        P_ERROR("[ERROR]: Erasing from address: %X, size: %d\n", t_addr, t_size);
        return SPIFFS_FLASH_BOUNDARY_ERROR;
    }
    osLogI(DBG_HAL_SPI,"esp_spiffs_erase:addr:%08x, size:%d \r\n",t_addr,t_size);
    // find sector number and offset from sector start
    uint16 sect_number = t_addr / SPIFFS_SECTOR_SIZE;
    uint32 sect_offset = t_addr % SPIFFS_SECTOR_SIZE;

    while (t_size > 0)
    {
        // P_TRACE("[TRACE]: bytes to be erased %d, sector num %d, sector offset %d\n",
        //         t_size, sect_number, sect_offset);
        // erase sector
        W25qxx_EraseSector(sect_number);
        // check if must delete more than one sector
        if (t_size > (SPIFFS_SECTOR_SIZE - sect_offset))
        {
            t_size -= (SPIFFS_SECTOR_SIZE - sect_offset);
            sect_number += 1;
            sect_offset = 0;
        }
        else
        {
            t_size = 0;
        }
    }
    return SPIFFS_OK;
}
