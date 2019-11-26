#ifndef IS25LP128F_H
#define IS25LP128F_H

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

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
void IS25LP128F_write(uint8_t *buf, uint32_t addr, uint32_t len);
void IS25LP128F_program(uint8_t *buf, uint32_t addr, uint32_t len);
uint8_t IS25LP128F_sfdp(uint8_t addr);

uint8_t IS25LP128F_status(void);      //05h
uint8_t IS25LP128F_bank_reg(void); //16h/C8h
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
