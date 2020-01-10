/**
 * @file    vfs_nvm.h
 * @brief   FAT 12/16 filesystem non-volatile memory management
 */

#ifndef VFS_NVM_H
#define VFS_NVM_H

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif


#define VFS_NVM_FAT_ADDR 0xFD0000u       // starting address for FAT table in the FLASH directory
#define VFS_NVM_FAT_SIZE 131072u         // maximum size for maximum number of 65425 clusters allowed

#define VFS_NVM_FILE_ADDR 0x1000         // starting address for storage of files in FLASH directory
#define VFS_NVM_FILE_SIZE (VFS_NVM_FAT_ADDR-VFS_NVM_FILE_ADDR)   // size for files in a FLASH directory

#define VFS_NVM_DIR_ADDR 0x000000u       // starting address for FLASH directory entry
#define VFS_NVM_DIR_SIZE 1024u           // size for 32 FatDirectoryEntries (32 bytes each)

#define VFS_NVM_FILE_CNT_MAX 26u             // max number of files allowed in the FLASH chip (25 programs + selector program)


void VFS_NVM_init(void);

uint8_t VFS_NVM_is_available(void);

void VFS_NVM_read_FAT(uint8_t *buf, uint32_t offset, uint32_t len);

void VFS_NVM_write_FAT(const uint8_t *buf, uint32_t offset, uint32_t len);

void VFS_NVM_setup_FAT(uint8_t *buf, uint32_t len);

void VFS_NVM_read_DIR(uint8_t *buf, uint32_t offset, uint32_t len);

void VFS_NVM_write_DIR(const uint8_t *buf, uint32_t offset, uint32_t len);

void VFS_NVM_setup_DIR(void);

void VFS_NVM_read_FILE(uint8_t *buf, uint32_t offset, uint32_t len);

void VFS_NVM_write_FILE(const uint8_t *buf, uint32_t offset, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif //VFS_NVM_H
