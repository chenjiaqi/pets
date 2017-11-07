/*
 * @File user_storage.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-06 10:41:02 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-06 19:08:29
 */

#include "user_storage.h"
#include "user_log.h"
#include "fstorage.h"
static user_storage_t user_storage_info;


static bool fs_call_back_flag = true;


#define PAGE_NUM 8
#define PAGE_SIZE 1024

#define CUR_REL_POS (user_storage_info.p_current_write_addr - user_storage_info.p_start_addr)


static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result);

FS_REGISTER_CFG(fs_config_t fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = PAGE_NUM,             // Number of physical flash pages required.
        .priority = 0xFE            // Priority for flash usage.
};


static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result)
{
    if (result != FS_SUCCESS)
    {
        //bsp_indication_set(BSP_INDICATE_FATAL_ERROR);
    }
    else
    {
        LOG_EVENT("fstorage command completed");
        fs_call_back_flag = true;
    }
}



#if 0 
static void store_temp_humity_to_flash()
{
    LOG_INFO("Current Store address is: %x", (unsigned int)(fs_config.p_start_addr + current_store_flash_point));
    //LOG_INFO("%x", *(fs_config.p_start_addr + current_store_flash_point));
    while(*(fs_config.p_start_addr + current_store_flash_point) != 0xFFFFFFFF)
    {
        current_store_flash_point++;
    }

    fs_call_back_flag = true;
    fs_store(&fs_config, fs_config.p_start_addr + current_store_flash_point, (uint32_t *)&user_flash_struct, sizeof(user_flash_struct)/sizeof(uint32_t), NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }
}
#endif

extern void power_manage();

void find_current_write_pos()
{
    //LOG_INFO("%X", user_storage_info.p_current_write_addr);
    if (CUR_REL_POS % PAGE_SIZE == 0)
    {
        //LOG_INFO("reach page");
        while(((*(user_storage_info.p_current_write_addr)) & 0x03) == 0x00)
        {
            LOG_INFO("next page");
            user_storage_info.p_current_write_addr += PAGE_SIZE;

            if (user_storage_info.p_current_write_addr == user_storage_info.p_start_addr + PAGE_NUM * PAGE_SIZE)
            {
                user_storage_info.p_current_write_addr = user_storage_info.p_start_addr;
            }
        }
        user_storage_info.p_current_write_addr += 8; // skep to page head;
    }

    //while((*(user_storage_info.p_current_write_addr)) != 0xffffffff && (CUR_REL_POS % PAGE_SIZE))
    while((CUR_REL_POS % PAGE_SIZE) &&(*(user_storage_info.p_current_write_addr)) != 0xffffffff )
    {
        user_storage_info.p_current_write_addr += 2;
    }

    if (CUR_REL_POS % PAGE_SIZE == 0)
    {
        if (CUR_REL_POS == (PAGE_SIZE * PAGE_NUM))
        {
            /** <Reach the end of storage> */
            user_storage_info.p_current_write_addr = user_storage_info.p_start_addr;
        }
        fs_erase(&fs_config, user_storage_info.p_current_write_addr, 1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
        LOG_INFO("Erase SUCCESS\r\n");
        user_storage_info.p_current_write_addr += 8; //skip page head
    }

    LOG_INFO("Current pos is %X:%X", LOG_UINT(user_storage_info.p_current_write_addr), *(user_storage_info.p_current_write_addr));
}

void user_storage_init()
{
    fs_init();
    user_storage_info.p_start_addr = fs_config.p_start_addr;
    user_storage_info.p_end_addr = fs_config.p_end_addr;
    user_storage_info.p_current_write_addr = fs_config.p_start_addr;
    LOG_INFO("%X--->%X",LOG_UINT(user_storage_info.p_start_addr), LOG_UINT(user_storage_info.p_end_addr));
    //fs_store(&fs_config, fs_config.p_start_addr + 22, &a,1, NULL);
    //find_current_write_pos();
}

void user_store_to_flash(user_flash_structure_t *info)
{
    find_current_write_pos();
    fs_call_back_flag = false;
    static uint32_t page_flag;
    fs_store(&fs_config, user_storage_info.p_current_write_addr, (uint32_t *)info, sizeof(*info) / (sizeof(uint32_t)), NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }
    LOG_INFO("Write Over");

    if (CUR_REL_POS % PAGE_SIZE == 8)
    {
        LOG_INFO("Write flag");
        page_flag = 0xfffffffe;
        fs_store(&fs_config, user_storage_info.p_current_write_addr - 8, &page_flag, 1, NULL);
        fs_call_back_flag = false;
        while (!fs_call_back_flag)
        {
            power_manage();
        }
    }

    if (CUR_REL_POS % PAGE_SIZE == PAGE_SIZE -2) // last record in a page
    {
        page_flag = 0xfffffffd;
        fs_call_back_flag = false;
        fs_store(&fs_config, user_storage_info.p_current_write_addr - 1022, &page_flag, 1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
    }
}

void user_storage_set_address(uint32_t p)
{
    user_storage_info.p_current_write_addr = user_storage_info.p_end_addr - 4;
    LOG_INFO("JUDGE POSITION %x", user_storage_info.p_current_write_addr);
}

uint32_t user_storage_get_a_record()
{
    if (user_storage_info.p_current_read_addr == 0x00) // first start up
    {
        uint32_t *p = user_storage_info.p_current_write_addr + (PAGE_SIZE - (CUR_REL_POS % PAGE_SIZE));
        LOG_INFO("%X", p);
        while(p != user_storage_info.p_end_addr && ((*p)&&0x03) &&( *(p+8) == 0xffffffff))
        {
            p = p + PAGE_SIZE;
        }
        LOG_INFO("%X", p);
    }
    else
    {
    }
}


