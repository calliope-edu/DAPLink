/**
 * @file    selectr.h
 * @brief   Handling of offline selection and programming of hex files from IS25LP128F flash memory
 */

#ifndef SELECTR_H
#define SELECTR_H

#include "stdint.h"
#include "virtual_fs.h"
//#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t selectr_start_mode(void);

void selectr_read_command(char command);

void selectr_write_command(char command);

uint8_t selectr_program_start(vfs_filename_t filename);

uint8_t selectr_program_handler(void);


#ifdef __cplusplus
}
#endif

#endif //SELECTR_H
