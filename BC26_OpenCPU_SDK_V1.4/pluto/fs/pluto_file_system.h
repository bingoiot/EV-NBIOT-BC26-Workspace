/*
 * jf_ini.h
 *
 *  Created on: 2016Äê3ÔÂ29ÈÕ
 *      Author: lort
 */

#ifndef PLUTO_FILE_SYSTEM_H_
#define PLUTO_FILE_SYSTEM_H_

#include "osCore.h"
#include "fs_entry.h"

typedef int		FILE ;

#define FLASH_SN_SECTOR_SIZE	(4096)
#define FLASH_SN_START_SECTOR	(0)//after 768KB storage user data
#define FLASH_SN_PAGE_SIZE 		(FLASH_SN_SECTOR_SIZE/4)
#define FLASH_SN_USE_FLAG		(0x5A5AA5A5)


extern char*		pluto_read_text_file(char *fname);
extern osState  	pluto_write_text_file(char *fname,char *text);


extern uint32 		pluto_get_file_length(char *fname);
extern uint32 		pluto_read_file(char *fname, uint8 *pdata, uint32 len);
extern osState 		pluto_write_file(char *fname, uint8 *pdata, uint32 len);

extern char*		pluto_get_file_name(char *filter, uint32 id);
extern osState 		pluto_delete_file(char *fname);
extern osState 		pluto_disk_format(void);

extern osState  	pluto_rename(char *pname,char *newname);

extern osState 		pluto_write_sn(char *str);
extern char* 		pluto_read_sn(void);

#endif /* JIFAN_JF_FS_JF_INI_H_ */
