#ifndef IS25LP128F_H
#define IS25LP128F_H

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS25LP128F_FAT_ADDR 0xFD0000u       // starting address for FAT table in the FLASH directory
#define IS25LP128F_FAT_SIZE 131072u         // maximum size for maximum number of 65425 clusters allowed

#define IS25LP128F_FILE_ADDR 0x1000         // starting address for storage of files in FLASH directory
#define IS25LP128F_FILE_SIZE (IS25LP128F_FAT_ADDR-IS25LP128F_FILE_ADDR)   // size for files in a FLASH directory

#define IS25LP128F_DIR_ADDR 0x000000u       // starting address for FLASH directory entry
#define IS25LP128F_DIR_SIZE 1024u           // size for 32 FatDirectoryEntries (32 bytes each)

#define IS25LP128F_FILE_MAX 26u             // max number of files allowed in the FLASH chip (25 programs + selector program)

#define IS25LP128F_MEM_SIZE_MB 16u
#define IS25LP128F_SECTOR_SIZE 4096u
#define IS25LP128F_PAGE_SIZE 256u


//status register cmd 0x05
#define IS25LP128F_WIP_MASK  1
#define IS25LP128F_WIP_BIT	 0

#define IS25LP128F_WEL_MASK  2
#define IS25LP128F_WEL_BIT   1

#define IS25LP128F_BP_MASK   (15<<2)
#define IS25LP128F_BP_BIT    2

#define IS25LP128F_QE_MASK   (1<<6)
#define IS25LP128F_QE_BIT    6

#define IS25LP128F_SRWD_MAKS (1<<7)
#define IS25LP128F_SRWD_BIT  7

//bank address register cmd 0x??
#define IS25LP128F_EXTADD_MASK (1<<7)
#define IS25LP128F_EXTADD_BIT  7

//function register
#define IS25LP128F_ESUS_MAKS (1<<3)
#define IS25LP128F_ESUS_BIT  3

#define IS25LP128F_PSUS_MAKS (1<<2)
#define IS25LP128F_PSUS_BIT  2

//Protos

void IS25LP128F_init(void);
uint8_t IS25LP128F_detect(void);
uint8_t IS25LP128F_is_detected(void);
void IS25LP128F_read(uint8_t *buf, uint32_t addr, uint32_t len);
void IS25LP128F_write(uint8_t const *buf, uint32_t addr, uint32_t len);
void IS25LP128F_write512(uint8_t const *buf, uint32_t addr, uint32_t len);

// Sector-wise destructive write
void IS25LP128F_write_sector(uint8_t const *buf, uint32_t addr, uint32_t len);

void IS25LP128F_program(uint8_t const *buf, uint32_t addr, uint32_t len);
uint8_t IS25LP128F_sfdp(uint8_t addr);

uint8_t IS25LP128F_status(void);      //05h
uint8_t IS25LP128F_bank_reg(void); //16h/C8h

// Wait while busy
void IS25LP128F_is_busy(void);

void IS25LP128F_delete_sector(uint32_t sec_add);
void IS25LP128F_delete_block(uint32_t blk_add);
void IS25LP128F_delete_chip(void);
void IS25LP128F_write_enable(void);
void IS25LP128F_suspend(void);
void IS25LP128F_resume(void);
uint8_t IS25LP128F_func_reg(void);
uint8_t IS25LP128F_extread_reg(void);
#ifdef __cplusplus
}
#endif

#endif
