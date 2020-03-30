/**
 * @file    vfs_nvm.c
 * @brief   FAT 12/16 filesystem non-volatile memory management
 */

#include "vfs_nvm.h"
#include "string.h"
#include "IS25LP128F.h"


void vfs_nvm_init(void){
    IS25LP128F_init();
    IS25LP128F_detect();
}

uint8_t vfs_nvm_is_available(void){
    return IS25LP128F_is_detected();
}

void vfs_nvm_read_FAT(uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_read(buf, VFS_NVM_FAT_ADDR+offset, len);
}


void vfs_nvm_write_FAT(const uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_write(buf, VFS_NVM_FAT_ADDR+offset, len);
}

void vfs_nvm_setup_FAT(void){
    uint32_t offset = 0u;
    uint8_t data[IS25LP128F_PAGE_SIZE];

    memset(data, 0, IS25LP128F_PAGE_SIZE);

    // initialize start of FAT in the FLASH memory with standard markers
    data[0] = 0xF8u;
    data[1] = 0xFFu;
    data[2] = 0xFFu;
    data[3] = 0xFFu;

    IS25LP128F_write(data, VFS_NVM_FAT_ADDR, IS25LP128F_PAGE_SIZE);

    // initialize remaining clusters with zero markers as a free cluster, see FAT specs
    memset(data, 0, IS25LP128F_PAGE_SIZE);

    for (offset = IS25LP128F_PAGE_SIZE; offset < (VFS_NVM_FAT_SIZE - IS25LP128F_PAGE_SIZE); offset += IS25LP128F_PAGE_SIZE)
    {
        IS25LP128F_write_sector(data, (VFS_NVM_FAT_ADDR+offset), IS25LP128F_PAGE_SIZE);
    }

    // initialize end of FAT in the FLASH memory with reserved markers
    // to let the USB host recognize correct size of the memory available
    memset(&(data[IS25LP128F_PAGE_SIZE-48u]), 0xFFu, 48u);

    IS25LP128F_write(data, (VFS_NVM_FAT_ADDR+VFS_NVM_FAT_SIZE-IS25LP128F_PAGE_SIZE), IS25LP128F_PAGE_SIZE);
}

void vfs_nvm_read_DIR(uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_read(buf, VFS_NVM_DIR_ADDR+offset, len);
}

void vfs_nvm_write_DIR(const uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_write(buf, VFS_NVM_DIR_ADDR+offset, len);
}

void vfs_nvm_setup_DIR(void){
    uint32_t offset = 0u;
    uint8_t data[IS25LP128F_PAGE_SIZE];

    memset(data, 0, IS25LP128F_PAGE_SIZE);

    for (offset = 0u; offset < VFS_NVM_DIR_SIZE; offset += IS25LP128F_PAGE_SIZE)
    {
        IS25LP128F_write_sector(data, (VFS_NVM_DIR_ADDR+offset), IS25LP128F_PAGE_SIZE);
    }
}

void vfs_nvm_read_FILE(uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_read(buf, VFS_NVM_FILE_ADDR+offset, len);
}

void vfs_nvm_write_FILE(const uint8_t *buf, uint32_t offset, uint32_t len){
    IS25LP128F_write_sector(buf, VFS_NVM_FILE_ADDR+offset, len);
}
