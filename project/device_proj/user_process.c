/*
 * @File undefined 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-30 17:11:54 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-30 17:17:54
 */
#include "user_process.h"
#include "user_log.h"
#include "fstorage.h"

/**@brief Function for User Process
 *
 * @details Deal with user process
 *
 */
extern bool is_need_write_flash ;
extern bool is_need_read_flash ;

static bool fs_call_back_flag = false;

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

FS_REGISTER_CFG(fs_config_t fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = 1,     // Number of physical flash pages required.
        .priority = 0xFE            // Priority for flash usage.
};
extern void power_manage();
void user_process(void)
{
    if(is_need_read_flash)
    {
        LOG_INFO("%x", fs_config.p_start_addr);
        for(int i = 0; i < 10; i++)
        {
            LOG_PROC("DATA","%x", *(fs_config.p_start_addr+i));
        }
        is_need_read_flash = false;
    }
    if(is_need_write_flash)
    {
        uint32_t err_code;
        static int current_pos = 0;
        LOG_INFO("write flash");
        LOG_INFO("%x", fs_config.p_start_addr);
        fs_call_back_flag = false;

        /*
        err_code = fs_erase(&fs_config,fs_config.p_start_addr,1,NULL);
        printf("%d", err_code);

        while(!fs_call_back_flag)
        {
            power_manage();
        }
        */
        LOG_INFO("erase success");
        static uint32_t data = 0xaabbccdd;
        fs_call_back_flag = true;
        fs_store(&fs_config, fs_config.p_start_addr + current_pos, &current_pos,1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
        LOG_INFO("write success");
        current_pos++;
        is_need_write_flash = false;
    }

}