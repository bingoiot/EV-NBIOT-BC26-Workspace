/*
 * jf_ini.c
 *
 *  Created on: 2016Äê3ÔÂ29ÈÕ
 *      Author: lort
 */

#include "pluto_file_system.h"
#include "fs_entry.h"
#include "w25qxx.h"

static osState  init_file_system(void);
static uint32 	get_file_size(char *fname);

char  *pluto_read_text_file(char *fname)
{
	FILE 			fd;
	char 			*pstr = NULL;
	char			*pname;
	uint32			slen = 0;
	uint32			flen;
	init_file_system();
	osLogI(DBG_FS,"pluto_read_text_file: \r\n");
	pname = (char*)fname;
	if(pname==NULL)return NULL;
	flen = get_file_size((char*)pname)+1;
	osLogI(DBG_FS,"pluto_read_text_file: %s, len:%d \r\n",pname,flen);
	if(flen>1)
	{
		fd = spifs_open(pname, O_RDWR);
		if(fd>=0)
		{
			pstr = (char *)osMalloc(flen+1);
			if(pstr!=NULL)
			{
				slen = spifs_read(fd,(uint8*)pstr,flen);
				if(slen<=0)
				{
					osFree(pstr);
					pstr = NULL;
				}
				pstr[flen] = 0x00;
				osLogI(DBG_FS,"pluto_read_text_file:lenth:%d\r\n%s\r\nend!\r\n",flen,pstr);
			}
			spifs_close(fd);
		}
		else
		{
			osLogI(DBG_ERROR,"pluto_read_text_file: open file :%s error \r\n",pname);
		}
	}
	return pstr;
}

osState  pluto_write_text_file(char *fname,char *str)
{
	FILE 			fd;
	char			*pname;
	uint32			slen;
	int				ret;
	init_file_system();
	pname = (char*)fname;
	if(pname==NULL)return osFailed;
	pluto_delete_file(pname);
	osLogI(DBG_FS,"pluto_write_text_file: %s\r\n",pname);
	fd = spifs_open(pname, O_CREAT|O_RDWR);
	if(fd>=0)
	{
		slen = osStrlen(str)+1;
		if((ret=spifs_write(fd,(uint8*)str,slen))>0)
		{
			spifs_close(fd);
			osLogI(DBG_FS,"pluto_write_text_file: len:%d ,\r\n%s \r\n",slen,str);
			return osSucceed;
		}
		else
			osLogI(DBG_ERROR,"pluto_write_text_file: fs_write file :%s error :%d \r\n",pname,ret);
		spifs_close(fd);
	}
	else
		osLogI(DBG_ERROR,"pluto_write_text_file: open file :%s error \r\n",pname);
	return osFailed;
}

uint32  pluto_get_file_length(char *fname)
{
	char *pname;
	uint32	slen = 0;
	init_file_system();
	pname = (char*)fname;
	if(pname==NULL)return 0;
	slen = get_file_size(pname);
	return slen;
}
uint32  pluto_read_file(char *fname, uint8 *pdata, uint32 len)
{
	FILE 		fd;
	char 		*pname;
	uint32 		slen = 0;
	init_file_system();
	pname = (char*)fname;
	if(pname==NULL)return 0;
	slen = get_file_size(pname);
	osLogI(DBG_FS,"pluto_read_file: %s, len:%d \r\n",pname,slen);
	if(slen>0)
	{
		osLogI(DBG_FS,"pluto_read_file: \r\n");
		fd = spifs_open(pname, O_RDWR);
		if(fd>=0)
		{
			slen = (slen>len)?len:slen;
			slen = spifs_read(fd,pdata,slen);
			if(slen>0)
			{
				spifs_close(fd);
				osLogB(DBG_FS,"pluto_read_file:",pdata,slen);
				return slen;
			}
			else
				osLogI(DBG_ERROR,"pluto_read_file: fs_read file :%s error \r\n",pname);
			spifs_close(fd);
		}
		else
			osLogI(DBG_ERROR,"pluto_read_file: open file :%s error \r\n",pname);
	}
	return slen;
}
osState  pluto_write_file(char *fname, uint8 *pdata, uint32 len)
{
	FILE 	fd;
	char	*pname;
	int 	slen = -1;
	init_file_system();
	pname = (char*)fname;
	if(pname==NULL)return osFailed;
	pluto_delete_file(pname);
	fd = spifs_open(pname, SPIFFS_O_CREAT|O_RDWR);
	if(fd>=0)
	{
		slen = spifs_write(fd,pdata,len);
		if(slen>=0)
		{
			spifs_close(fd);
			osLogB(DBG_FS,"pluto_write_file:",pdata,slen);
			return osSucceed;
		}
		else
			osLogI(DBG_ERROR,"pluto_write_file: fs_write file :%s error \r\n",pname);
		spifs_close(fd);
	}
	else
		osLogI(DBG_ERROR,"pluto_write_file: open file :%s error \r\n",pname);
	return osFailed;
}
char *pluto_get_file_name(char *filter, uint32 id)
{
	char			*bPath;
	char			*fname = NULL;
	char			*p = NULL;
	spiffs_DIR 		dir;
	spiffs_DIR		*pdir;
	struct 	spiffs_dirent 	tr;
	struct 	spiffs_dirent 	*ptr;
	uint32	cnt = 0;
	uint8 	match = 0x00;
	uint8	match_flag;
	init_file_system();
	osLogI(DBG_FS,"pluto_get_file_name : \r\n");
	bPath = "/";
	if(bPath!=NULL)
	{
		osLogI(DBG_FS,"pluto_get_file_name: Open dir:%s\r\n",bPath);
		if ((pdir=spifs_opendir(bPath,&dir)) != NULL)
		{
			if((p = osStrchr(filter,'*'))!=NULL)
			{
				if(p[1]=='\0')//match all file
				{
					match = 0xFF;
				}
				else
				{
					match =0x01;
					p++;
				}
			}
			osLogI(DBG_FS,"try read dir id:%d \r\n",cnt);
			while ((ptr=spifs_readdir(pdir,&tr)) != NULL)
			{
				match_flag = osFalse;
				osLogI(DBG_FS,"read dir succeed type:%d,name:%s  filter:%s \r\n",ptr->type,ptr->name,filter);
				switch(match)
				{
				case 0x00://match name
					if(osStrcmp(ptr->name,(char*)filter)==0)
					{
						match_flag = osTrue;
					}
					break;
				case 0x01://match type
					if(osCaseStrstr(ptr->name,(char*)p)!=NULL)
					{
						match_flag = osTrue;
					}
					break;
				case 0xFF://match all file
					match_flag = osTrue;
					break;
				}
				if(match_flag == osTrue)
				{
					osLogI(DBG_FS,"try read dir filter pass, id = %d name:%s \r\n",id,ptr->name);
					if(cnt==id)
					{
						fname = (char*)osMalloc(osStrlen(ptr->name)+1);
						if(fname!=NULL)
						{
							osStrcpy(fname,ptr->name);
						}
						break;
					}
					cnt++;
				}
			}
			spifs_closedir(pdir);
		}
		else
			osLogI(DBG_FS,"Open dir error...\r\n");
	}
	return fname;
}
osState pluto_delete_file(char *fname)
{
	char *bPath = (char*)fname;
	init_file_system();
	if(bPath!=NULL)
	{
		if(spifs_remove(bPath)>=0)
		{
			osLogI(DBG_FS,"pluto_delete_file succeed!-->%s\r\n",bPath);
			return osSucceed;
		}
	}
	return osFailed;
}
osState pluto_delete_all_file(void)
{
	spifs_format();
	return osSucceed;
}
osState pluto_disk_format(void)
{
	osLogI(DBG_FS,"pluto_disk_format\r\n");
	init_file_system();
	spifs_format();
	return osSucceed;
}
osState   pluto_rename(char *pname,char *newname)
{
	char *pold,*pnew;
	init_file_system();
	pold = (char*)pname;
	pnew = (char*)newname;
	if((pold==NULL)||(pnew==NULL))return osFailed;
	spifs_rename(pold,pnew);
	return osSucceed;
}
static osState init_file_system(void)
{
	static uint8 init_flag = osFalse;
	if(init_flag==osTrue)
		return osFailed;
	init_flag = osTrue;
	osLogI(DBG_FS, "Initializing SPIFFS\r\n");
	spifs_init();
	size_t total = 0, used = 0;
	spifs_info(&total, &used);
	osLogI(DBG_COMMON, "Partition size: total: %d, used: %d \r\n", total, used);
	return osSucceed;
}
static uint32  get_file_size(char *fname)
{
	spiffs_stat  	Stat;
	uint32	slen = 0;
	init_file_system();
	if(spifs_stat(fname,&Stat)>=0)
	{
		slen = Stat.size;
		osLogI(DBG_FS,"ini_getFileLenth:len%d \r\n",slen);
	}
	else
		osLogI(DBG_FS,"ini_getFileLenth: get file:%s state error \r\n",fname);
	return slen;
}

#define FILE_SN_HEAD_SIZE 		(sizeof(uint32)+sizeof(uint16)+sizeof(uint16))
typedef struct
{
	uint32 	useflag;
	uint16	crc;
	uint16	len;
	uint8	sn[FLASH_SN_PAGE_SIZE-FILE_SN_HEAD_SIZE];
}sn_table_t;

osState FUC_ATTR pluto_write_sn(char *str)
{
	sn_table_t 			*psn;
	uint32				dlen;
	int 				i;
	init_file_system();
	dlen = osStrlen(str);
	osLogI(DBG_SN,"write_sn:%s\r\n",str);
	if(dlen<(FLASH_SN_PAGE_SIZE-FILE_SN_HEAD_SIZE))
	{
		W25qxx_EraseSector(FLASH_SN_START_SECTOR);
		psn = (sn_table_t*)osMalloc(sizeof(sn_table_t));
		if(psn!=NULL)
		{
			osMemset(psn,0x00,sizeof(sn_table_t));
			psn->useflag = FLASH_SN_USE_FLAG;
			psn->len = dlen;
			psn->crc = osCRC16(0,(uint8*)str,dlen);
			osMemcpy(psn->sn,(uint8*)str,dlen);
			for(i=(FLASH_SN_START_SECTOR*FLASH_SN_SECTOR_SIZE);i<((FLASH_SN_START_SECTOR+1)*FLASH_SN_SECTOR_SIZE);i+=FLASH_SN_PAGE_SIZE)
			{
				osLogI(DBG_SN,"pluto_write_sn:sec:%d, offset:%d len:%d \r\n",(i/FLASH_SN_SECTOR_SIZE),(i%FLASH_SN_SECTOR_SIZE),sizeof(sn_table_t));
				W25qxx_WriteSector((uint8 *)psn,(i/FLASH_SN_SECTOR_SIZE),(i%FLASH_SN_SECTOR_SIZE),sizeof(sn_table_t));
			}
			osFree(psn);
		}
		else
			osLogI(DBG_ERROR,"write_sn: malloc mem error !  \r\n ");
	}
	return 0;
}
char* FUC_ATTR pluto_read_sn(void)
{
	sn_table_t 			*psn = NULL;
	char				*pbuf;
	int 				i;
	uint16 				crc;
	init_file_system();
	psn = (sn_table_t *)osMalloc(sizeof(sn_table_t));
	if(psn!=NULL)
	{
		for(i=(FLASH_SN_START_SECTOR*FLASH_SN_SECTOR_SIZE);i<((FLASH_SN_START_SECTOR+1)*FLASH_SN_SECTOR_SIZE);i+=FLASH_SN_PAGE_SIZE)
		{
			osLogI(DBG_SN,"pluto_sn_read: addr: %d \r\n",i);
			W25qxx_ReadSector((uint8*)psn,(i/FLASH_SN_SECTOR_SIZE),(i%FLASH_SN_SECTOR_SIZE),sizeof(sn_table_t));
			if(psn->useflag==FLASH_SN_USE_FLAG)
			{
				crc = osCRC16(0,psn->sn,psn->len);
				if(crc==psn->crc)
				{
					pbuf = (char*)psn;
					i = psn->len;
					osMemcpy(pbuf,psn->sn,psn->len);
					pbuf[i] = '\0';
					return pbuf;
				}
			}
		}
		osFree(psn);
	}
	return NULL;
}

