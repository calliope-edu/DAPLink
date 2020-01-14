/**
 * @file    selectr.c
 * @brief   Handling of offline selection and programming of hex files from IS25LP128F flash memory
 */

#include "selectr.h"
#include "string.h"
#include "vfs_nvm.h"
#include "util.h"
#include "uart.h"
#include "file_stream.h"
#include "main.h"

#define SELECTR_BUF_SIZE 256u


static uint8_t selector_mode = 0u;
static uint8_t file_idx = 0u;
static uint8_t filenames_found = 0u;
static vfs_filename_t filenames[VFS_NVM_FILE_CNT_MAX];

static uint16_t cluster_count = 0u;
static uint32_t file_size = 0u;
static uint16_t cluster = 0u;

static uint16_t cluster_counter = 0u;
static uint32_t program_size = 0u;
static uint32_t page_address = 0u;
static uint32_t sector_address = 0u;
static uint8_t program_buf[SELECTR_BUF_SIZE];
static uint8_t program_flag = 0u;

extern uint32_t fat_idx;

uint8_t selectr_start_mode(void)
{
    uint8_t result = 0u;

    // Program target micro with "selector" software
    if (selectr_program_start("SELECTR HEX") != 0u)
    {
        file_idx = 0u;
        filenames_found = 0u;
        selector_mode = 1u;
        memset(filenames, 0, VFS_NVM_FILE_CNT_MAX*(sizeof(vfs_filename_t)));

        result = 1u;
    }
    else
    {
        result = 0u;
    }

    return result;
}

void selectr_read_command(char command)
{
    if (selector_mode != 0u)
    {
        if (command == 'R')
        {
            // Target micro loaded with "selector" software and ready for operation
            filenames_found = vfs_get_flash_names_srtd(filenames, VFS_NVM_FILE_CNT_MAX);

            if (filenames_found > 0u)
            {
                file_idx = 1u; // select first program file by default
                selectr_write_command(file_idx);
            }
            else
            {
                selectr_write_command(13u); // FLASH dir is empty, blink central LED
            }
        }
        else if (command == 'A')
        {
            // Button A pressed
            if (file_idx > 1u)
            {
                file_idx --;
                selectr_write_command(file_idx);
            }
            else
            {
                if (filenames_found > 0u)
                {
                    file_idx = filenames_found;
                    selectr_write_command(file_idx);
                }
                else
                {
                    selectr_write_command(13u); // FLASH dir is empty, blink central LED
                }
            }
        }
        else if (command == 'B')
        {
            // Button B pressed
            if ( file_idx < filenames_found )
            {
                file_idx ++;
                selectr_write_command(file_idx);
            }
            else
            {
                if (filenames_found > 0u)
                {
                    file_idx = 1u;
                    selectr_write_command(file_idx);
                }
                else
                {
                    selectr_write_command(13u); // FLASH dir is empty, blink central LED
                }
            }
        }
        else if ((command == 'C') || (command == 'S'))
        {
            // Buttons A+B pressed OR Shake detected
            if (file_idx != 0u)
            {
                selector_mode = 0u;

                if (selectr_program_start(filenames[file_idx-1u]) != 0u)
                {

                }
                else
                {
                    selectr_write_command(13u); // FLASH dir is empty, blink central LED
                }
            }
            else
            {
                selectr_write_command(13u); // FLASH dir is empty, blink central LED
            }
        }
        else
        {
            // ignore
        }
    }
    else
    {
        // ignore
    }
}

void selectr_write_command(char command)
{
    uint8_t data[3] = {(uint8_t)command, 0x0Du, 0x0Au};
    util_write_uint32((char*)(&(data[0])), (uint8_t)command);
    uart_write_data(data, 3u);
}

uint8_t selectr_program_start(vfs_filename_t filename)
{
    uint8_t result = 0u;

    stream_type_t stream_type = STREAM_TYPE_NONE;

    stream_type = stream_type_from_name(filename);

    if (stream_type != STREAM_TYPE_NONE)
    {
        if (vfs_find_flash_file(filename, &cluster, &file_size) != 0u)
        {
            stream_open(stream_type);

            cluster_count = (file_size + VFS_CLUSTER_SIZE - 1u) / VFS_CLUSTER_SIZE;
            cluster_counter = 0u;
            program_size = 0u;
            page_address = 0u;
            sector_address = ((cluster - fat_idx) * VFS_CLUSTER_SIZE);

            program_flag = 1u;

            result = 1u;
        }
        else
        {
            result = 0u;
        }
    }
    else
    {
        result = 0u;
    }

    return result;
}

uint8_t selectr_program_handler(void)
{
    uint8_t result = 0u;

    if (program_flag != 0u)
    {
        //for (cluster_cnt = 0u; cluster_cnt < cluster_count; cluster_cnt++)
        if (cluster_counter < cluster_count)
        {
            if (cluster >= 0xFFF8u)
            {
                //end of chain, last cluster in file
                //terminate the loop
                cluster_counter = cluster_count;
            }
            else if (cluster >= 0x0002u)
            {
                main_blink_hid_led(MAIN_LED_FLASH);
                //data cluster, use it to read next part of the file

                if (page_address < VFS_CLUSTER_SIZE)
                {
                    if (program_size + SELECTR_BUF_SIZE < file_size)
                    {
                        vfs_nvm_read_FILE(program_buf, sector_address + page_address, SELECTR_BUF_SIZE);
                        stream_write(program_buf, SELECTR_BUF_SIZE);
                        program_size += SELECTR_BUF_SIZE;
                        page_address += SELECTR_BUF_SIZE;
                    }
                    else
                    {
                        vfs_nvm_read_FILE(program_buf, sector_address + page_address, (file_size-program_size));
                        stream_write(program_buf, (file_size-program_size));
                        program_size += (file_size-program_size);
                        page_address += (file_size-program_size);

                        //filesize reached, stop further reading/streaming
                        //terminate the loop
                        cluster_counter = cluster_count;
                        page_address = 0u;
                    }
                }
                else
                {
                    //read next cluster number
                    vfs_nvm_read_FAT(program_buf, (cluster * 2u), 2u);

                    cluster = program_buf[1];
                    cluster <<= 8u;
                    cluster |= program_buf[0];

                    sector_address = ((cluster - fat_idx) * VFS_CLUSTER_SIZE);

                    cluster_counter++;
                    page_address = 0u;
                }
            }
            else
            {
                //invalid, reserved cluster
                //terminate the loop
                cluster_counter = cluster_count;
            }

            result = 1u;

        }
        else if (cluster_counter == cluster_count)
        {
            stream_close();

            program_flag = 0u;

            //run this block once
            cluster_counter++;

            uart_reset();

            main_reset_target(0u);

            result = 0u;
        }
        else
        {
            result = 0u;
        }
    }
    else
    {
        result = 0u;
    }

    return result;
}
