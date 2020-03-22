/*
 * fs_entry.h
 *
 *  Created on: 2016Äê12ÔÂ25ÈÕ
 *      Author: zigbee
 */

#ifndef SPIFFS_FS_ENTRY_H_
#define SPIFFS_FS_ENTRY_H_

#define USE_OPTIMIZE_PRINTF

#include "spiffs.h"
/*
#undef SPI_FLASH_SIZE_MAP
#define SPI_FLASH_SIZE_MAP 		4

#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE 0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR 0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR 0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR 0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR 0xfd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR 0x7c000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE 0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR 0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR 0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR 0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR 0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR 0x7c000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE 0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR 0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR 0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR 0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR 0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR 0x7c000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE 0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR 0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR 0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR 0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR 0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR 0xfc000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE 0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR 0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR 0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR 0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR 0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR 0xfc000
#else
#error "The flash map is not supported"
#endif
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM 	SYSTEM_PARTITION_CUSTOMER_BEGIN

#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "There is no room for spiffs"
#elif (SPI_FLASH_SIZE_MAP == 2)
#error "There is no room for spiffs"
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_DATA 0x101000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_DATA 0x101000
#elif (SPI_FLASH_SIZE_MAP == 5)
#error "There is no room for spiffs"
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_DATA 0x201000
#else
#error "The flash map is not supported"
#endif
*/

#define SPIFFS_LOG_PAGE_SIZE		(256)
#define SPIFFS_SECTOR_SIZE 			(1024 * 4)
#define SPIFFS_START_ADDR 			(0+(SPIFFS_SECTOR_SIZE*8))//remain front 8th sector
#define SPIFFS_END_ADDR 			(SPIFFS_START_ADDR+(1024*50))//remain after 8th sector
#define FS_ALIGN_BYTES 4
#define SPIFFS_FLASH_RESULT_ERR 	-10200
#define SPIFFS_FLASH_RESULT_TIMEOUT -10201
#define SPIFFS_FLASH_BOUNDARY_ERROR -10202

// debug macros
#define debug (1)
#if debug
#define P_FATAL(...) 	os_printf(__VA_ARGS__)
#define P_ERROR(...) 	os_printf(__VA_ARGS__)
#define P_WARN(...) 	os_printf(__VA_ARGS__)
#define P_INFO(...) 	os_printf(__VA_ARGS__)
#define P_DEBUG(...) 	os_printf(__VA_ARGS__)
#define P_TRACE(...) 	os_printf(__VA_ARGS__)
#define P_ALL(...) 		os_printf(__VA_ARGS__)
#else
#define P_FATAL(...) \
  {                  \
  }
#define P_ERROR(...) \
  {                  \
  }
#define P_WARN(...) \
  {                 \
  }
#define P_INFO(...) \
  {                 \
  }
#define P_DEBUG(...) \
  {                  \
  }
#define P_TRACE(...) \
  {                  \
  }
#define P_ALL(...) \
  {                \
  }
#endif

#define O_APPEND  	SPIFFS_O_APPEND
#define O_TRUNC  	SPIFFS_O_TRUNC
#define O_CREAT 	SPIFFS_O_CREAT
#define O_RDWR 		SPIFFS_O_RDWR

int 	spifs_init(void);
int  	spifs_format(void);
int 	spifs_open(char *path, uint32 flags);
int 	spifs_close(int fd);
int 	spifs_write(int fd, uint8 *pdata, int len);
int  	spifs_read(int fd, uint8 *pdata, int len);

int  					spifs_stat(char *path,spiffs_stat *pstat);
spiffs_DIR 				*spifs_opendir(char *path, spiffs_DIR *dir);
struct spiffs_dirent 	*spifs_readdir(spiffs_DIR *dir, struct spiffs_dirent *dirent);
int 					spifs_closedir(spiffs_DIR *dir);
int 	spifs_remove(char *path);
int  	spifs_rename(char *old_path, char *new_path);
int 	spifs_info(uint32* total, uint32 *used);

#endif /* SPIFFS_FS_ENTRY_H_ */
