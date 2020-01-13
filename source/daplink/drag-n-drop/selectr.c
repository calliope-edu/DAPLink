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

static uint8_t selector_mode = 0u;
static uint8_t file_idx = 0u;
static uint8_t filenames_found = 0u;
static vfs_filename_t filenames[VFS_NVM_FILE_CNT_MAX];

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
            filenames_found = vfs_get_names_srtd(filenames, VFS_NVM_FILE_CNT_MAX);

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

#define FF_FLASH_BUF 256u

static uint16_t ff_cluster_count = 0u;
static uint32_t ff_filesize = 0u;
static uint16_t ff_cluster = 0u;

static uint16_t ff_cluster_counter = 0u;
static uint32_t ff_size = 0u;
static uint32_t ff_page_address = 0u;
static uint32_t ff_sector_address = 0u;
static uint8_t ff_buf[FF_FLASH_BUF];
static uint8_t ff_start = 0u;
extern uint32_t fat_idx;
uint8_t selectr_program_start(vfs_filename_t filename)
{
    uint8_t result = 0u;

    stream_type_t stream_type = STREAM_TYPE_NONE;

    stream_type = stream_type_from_name(filename);

    if (stream_type != STREAM_TYPE_NONE)
    {
        if (vfs_find_file(filename, &ff_cluster, &ff_filesize) != 0u)
        {
            stream_open(stream_type);

            ff_cluster_count = (ff_filesize + VFS_CLUSTER_SIZE - 1u) / VFS_CLUSTER_SIZE;
            ff_cluster_counter = 0u;
            ff_size = 0u;
            ff_page_address = 0u;
            ff_sector_address = ((ff_cluster - fat_idx) * VFS_CLUSTER_SIZE);

            ff_start = 1u;

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

    if (ff_start != 0u)
    {
        //for (cluster_cnt = 0u; cluster_cnt < cluster_count; cluster_cnt++)
        if (ff_cluster_counter < ff_cluster_count)
        {
            if (ff_cluster >= 0xFFF8u)
            {
                //end of chain, last cluster in file
                //terminate the loop
                ff_cluster_counter = ff_cluster_count;
            }
            else if (ff_cluster >= 0x0002u)
            {
                main_blink_hid_led(MAIN_LED_FLASH);
                //data cluster, use it to read next part of the file

                if (ff_page_address < VFS_CLUSTER_SIZE)
                {
                    if (ff_size + FF_FLASH_BUF < ff_filesize)
                    {
                        vfs_nvm_read_FILE(ff_buf, ff_sector_address + ff_page_address, FF_FLASH_BUF);
                        stream_write(ff_buf, FF_FLASH_BUF);
                        ff_size += FF_FLASH_BUF;
                        ff_page_address += FF_FLASH_BUF;
                    }
                    else
                    {
                        vfs_nvm_read_FILE(ff_buf, ff_sector_address + ff_page_address, (ff_filesize-ff_size));
                        stream_write(ff_buf, (ff_filesize-ff_size));
                        ff_size += (ff_filesize-ff_size);
                        ff_page_address += (ff_filesize-ff_size);

                        //filesize reached, stop further reading/streaming
                        //terminate the loop
                        ff_cluster_counter = ff_cluster_count;
                        ff_page_address = 0u;
                    }
                }
                else
                {
                    //read next cluster number
                    vfs_nvm_read_FAT(ff_buf, (ff_cluster * 2u), 2u);

                    ff_cluster = ff_buf[1];
                    ff_cluster <<= 8u;
                    ff_cluster |= ff_buf[0];

                    ff_sector_address = ((ff_cluster - fat_idx) * VFS_CLUSTER_SIZE);

                    ff_cluster_counter++;
                    ff_page_address = 0u;
                }
            }
            else
            {
                //invalid, reserved cluster
                //terminate the loop
                ff_cluster_counter = ff_cluster_count;
            }

            result = 1u;

        }
        else if (ff_cluster_counter == ff_cluster_count)
        {
            stream_close();

            ff_start = 0u;

            //run this block once
            ff_cluster_counter++;

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
