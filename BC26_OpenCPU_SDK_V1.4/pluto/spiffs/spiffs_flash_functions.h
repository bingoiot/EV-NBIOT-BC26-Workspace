/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <quackmore-ff@yahoo.com> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you 
 * think this stuff is worth it, you can buy me a beer in return. Quackmore
 * ----------------------------------------------------------------------------
 */
#ifndef __SPIFFS_FLASH_FUNTIONS_H__
#define __SPIFFS_FLASH_FUNTIONS_H__

// ESP8266 and SDK references
#include "spiffs.h" // for spiffs types

// flash memory functions (checkout SPIFFS doumentation)
s32_t esp_spiffs_read(u32_t t_addr, u32_t t_size, u8_t *t_dst);
s32_t esp_spiffs_write(u32_t t_addr, u32_t t_size, u8_t *t_src);
s32_t esp_spiffs_erase(u32_t t_addr, u32_t t_size);

#endif
