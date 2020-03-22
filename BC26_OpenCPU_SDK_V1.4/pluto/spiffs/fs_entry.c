/*
 * fs_entry.c
 *
 *  Created on: 2016Äê12ÔÂ25ÈÕ
 *      Author: zigbee
 */

#include "osCore.h"
#include "spiffs_config.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs_flash_functions.h"
#include "fs_entry.h"
#include "w25qxx.h"

#define SPIFFS_FDS_SIZE			(32*4)
#define SPIFFS_CACHE_BUF_SIZE	((SPIFFS_LOG_PAGE_SIZE+32)*4)

typedef struct
{
	u8_t spiffs_work_buf[SPIFFS_LOG_PAGE_SIZE*2];
	u8_t spiffs_fds[SPIFFS_FDS_SIZE];
	u8_t spiffs_cache_buf[SPIFFS_CACHE_BUF_SIZE];
}spiffs_buffer_t;

static spiffs 			spi_fs;
static spiffs_config 	spi_fs_cfg;
static spiffs_buffer_t 	*spiffs_buffer;
static int spiffs_mount(void);
//int test_file_system(void);
int  spifs_init(void)
{
	os_printf("spiffs_init: start....\r\n");
	W25qxx_Init();
	W25qxx_BlockUnlock();
	spiffs_buffer = (spiffs_buffer_t*)osMalloc(sizeof(spiffs_buffer_t));
	if(spiffs_buffer==NULL)
		return -1;
	if(spiffs_mount()!=0)
	{
		os_printf("spiffs_init: unmount....\r\n");
		SPIFFS_unmount(&spi_fs);
		os_printf("spiffs_init: format....\r\n");
		SPIFFS_format(&spi_fs);
		if(spiffs_mount()!=0)
		{
			os_printf("spiffs_init: mount error!....\r\n");
			return -1;
		}
	}
	//test_file_system();
	os_printf("spiffs_init: mount succeed!....\r\n");
	return 0;
}
int  spifs_format(void)
{
	SPIFFS_unmount(&spi_fs);
	os_printf("spiffs_init: format....\r\n");
	SPIFFS_format(&spi_fs);
	if(spiffs_mount()!=0)
	{
		os_printf("spiffs_init: mount error!....\r\n");
		return -1;
	}
	return 0;
}
int  spifs_open(char *path, uint32 flags)
{
	return SPIFFS_open(&spi_fs, path, flags, 0);
}
int  spifs_close(int fd)
{
	return SPIFFS_close(&spi_fs, fd);
}
int  spifs_write(int fd, uint8 *pdata, int len)
{
	return SPIFFS_write(&spi_fs,fd,pdata,len);
}
int  spifs_read(int fd, uint8 *pdata, int len)
{
	return SPIFFS_read(&spi_fs,fd,pdata,len);
}
int  spifs_stat(char *path,spiffs_stat *pstat)
{
	return SPIFFS_stat(&spi_fs, path, pstat);
}

spiffs_DIR  *spifs_opendir(char *path, spiffs_DIR *dir)
{
	return SPIFFS_opendir(&spi_fs,path,dir);
}
struct spiffs_dirent  *spifs_readdir(spiffs_DIR *dir, struct spiffs_dirent *dirent)
{
	return SPIFFS_readdir(dir, dirent);
}
int  spifs_closedir(spiffs_DIR *dir)
{
	return SPIFFS_closedir(dir);
}
int  spifs_remove(char *path)
{
	return SPIFFS_remove(&spi_fs,path);
}
int  spifs_rename(char *old_path, char *new_path)
{
	return SPIFFS_rename(&spi_fs,old_path,new_path);
}
int  spifs_info(uint32* total, uint32 *used)
{
    return SPIFFS_info(&spi_fs, total, used);
}


static int
spiffs_mount(void) {
  spi_fs_cfg.phys_size = (SPIFFS_END_ADDR-SPIFFS_START_ADDR); // use all spi flash
  spi_fs_cfg.phys_addr = SPIFFS_START_ADDR; // start spiffs at start of spi flash

  spi_fs_cfg.phys_erase_block = SPIFFS_SECTOR_SIZE;
  spi_fs_cfg.log_block_size = SPIFFS_SECTOR_SIZE;
  spi_fs_cfg.log_page_size = SPIFFS_LOG_PAGE_SIZE;

  spi_fs_cfg.hal_read_f = esp_spiffs_read;
  spi_fs_cfg.hal_write_f = esp_spiffs_write;
  spi_fs_cfg.hal_erase_f = esp_spiffs_erase;

  int res = SPIFFS_mount(&spi_fs,
          &spi_fs_cfg,
		  spiffs_buffer->spiffs_work_buf,
		  spiffs_buffer->spiffs_fds,
		  SPIFFS_FDS_SIZE,
		  spiffs_buffer->spiffs_cache_buf,
		  SPIFFS_CACHE_BUF_SIZE,
          0);
  os_printf("mount res: %d \n", res);
  return res;
}

/*
#define ppsize (1024+100)
static ppbuf[ppsize];
int  test_file_system(void)
{
	int res,i;
	spiffs_stat stat;
	spiffs_file fd;
	os_printf("test_file_system: open file test_file.c !....\r\n");
	fd = SPIFFS_open(&spi_fs, "test_file.c", SPIFFS_CREAT|SPIFFS_APPEND|SPIFFS_RDWR, 0);
	if(fd>=0)
	{
	  for(i=0;i<1000;i++)
	  {
		  os_memset(ppbuf,i+1,ppsize);
		  res = SPIFFS_write(&spi_fs,fd,ppbuf,ppsize);
		  os_printf(" test_file_system write res: %d \r\n",res);
		  res = SPIFFS_fstat(&spi_fs, fd, &stat);
		  if (res < 0) {
			os_printf(" test_file_system write failed fstat, %d \r\n",res);
		  }
		  os_printf(" test_file_system write succeed size, %d \r\n", stat.size);
	  }
	  SPIFFS_close(&spi_fs, fd);
	}
	else
	{
		os_printf("test_file_system: open file test_file.c failed !....\r\n");
	}

	fd = SPIFFS_open(&spi_fs, "test_file.c", SPIFFS_CREAT|SPIFFS_RDWR, 0);
	if(fd>=0)
	{
	  for(i=0;i<100;i++)
	  {
		  os_memset(ppbuf,i+1,ppsize);
		  res = SPIFFS_read(&spi_fs,fd,ppbuf,ppsize);
		  if (res < 0) {
			os_printf(" test_file_system read failed fstat, %d \r\n",res);
		  }
		  os_printf(" test_file_system read succeed size, %d \r\n", res);
	  }
	  SPIFFS_close(&spi_fs, fd);
	}
	else
	{
		os_printf("test_file_system: open file test_file.c failed !....\r\n");
	}
	return 0;
}
*/
