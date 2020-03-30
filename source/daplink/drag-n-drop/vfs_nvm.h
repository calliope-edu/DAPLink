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


/* NVM MEMORY LAYOUT
 * START ADDR   -   NAME           - SIZE
 * 0x00000000   -   DIR REGION     - 0x00001000
 * 0x00001000   -   DATA REGION    - 0x00FCF000
 * 0x00FD0000   -   FAT REGION     - 0x00020000
 * 0X00FF0000   -   REWRITE BUFFER - 0X00010000
 */

#define VFS_NVM_FAT_ADDR 0xFEE000u                              // starting address for FAT table in the FLASH directory
#define VFS_NVM_FAT_SIZE 8192u                                  // maximum size for maximum number of 65425 clusters allowed

#define VFS_NVM_FILE_ADDR 0x1000                                // starting address for storage of files in FLASH directory
#define VFS_NVM_FILE_SIZE (VFS_NVM_FAT_ADDR-VFS_NVM_FILE_ADDR)  // size for files in a FLASH directory

#define VFS_NVM_DIR_ADDR 0x000000u                              // starting address for FLASH directory entry
#define VFS_NVM_DIR_SIZE (VFS_NVM_FILE_ADDR-VFS_NVM_DIR_ADDR)   // size for 128 FatDirectoryEntries (32 bytes each)


void vfs_nvm_init(void);

uint8_t vfs_nvm_is_available(void);

void vfs_nvm_read_FAT(uint8_t *buf, uint32_t offset, uint32_t len);

void vfs_nvm_write_FAT(const uint8_t *buf, uint32_t offset, uint32_t len);

void vfs_nvm_setup_FAT(uint8_t *buf, uint32_t len);

void vfs_nvm_read_DIR(uint8_t *buf, uint32_t offset, uint32_t len);

void vfs_nvm_write_DIR(const uint8_t *buf, uint32_t offset, uint32_t len);

void vfs_nvm_setup_DIR(void);

void vfs_nvm_read_FILE(uint8_t *buf, uint32_t offset, uint32_t len);

void vfs_nvm_write_FILE(const uint8_t *buf, uint32_t offset, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif //VFS_NVM_H
