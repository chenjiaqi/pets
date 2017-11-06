/*
 * @File undefined 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-30 17:11:54 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-06 17:54:47
 */
#include "user_process.h"
#include "user_log.h"
#include "fstorage.h"
#include "global.h"
#include "MyDHT11.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "user_ble_device_manages.h"
#include "nrf_temp.h"
#include "nrf_soc.h"
#include "user_storage.h"
/**@brief Function for User Process
 *
 * @details Deal with user process
 *
 */
extern bool is_need_write_flash ;
extern bool is_need_read_flash ;



static bool fs_call_back_flag = false;


static user_flash_structure_t user_flash_struct;

#if 0 
static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result);
FS_REGISTER_CFG(fs_config_t fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = 80,             // Number of physical flash pages required.
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

#if 0
static void start_to_trans_data_fo_app()
{
    uint32_t err_code;
    static uint32_t current_position = 0;
    current_position = current_store_flash_point;
    LOG_PROC("--->","START");
    //for(int i = 0; i < current_position; i += 2)
    for (int i = 0; i < current_position / 2; i += 2)
    {
        do
        {
            err_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *)(fs_config.p_start_addr + i * 2), 16);
            if (err_code == NRF_ERROR_INVALID_STATE)
            {
                LOG_ERROR("BLE disconnect or notification off");
                return;
            }

        } while (err_code != NRF_SUCCESS);
    }

    LOG_PROC("--->","OVER %lx",LOG_UINT(fs_config.p_start_addr + current_position));
}
#endif


extern void power_manage();
extern bool is_need_read;
void user_process(void)
{
    if(is_need_read_flash)
    {
        //start_to_trans_data_fo_app();

        

#if 0
        LOG_PROC("--->","START");
        uint32_t err_code;
        static uint8_t data[16];
        for(int j = 0; j < 100; j++)
        {
        for(int i = 0; i < 10; i++)
        {
            
            err_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *) fs_config.p_start_addr, 16);
            //err_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *) data, 16);
            //LOG_ERROR("%X", err_code);
            while(err_code != NRF_SUCCESS)
            {
                //err_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *) data, 16);
                err_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *) fs_config.p_start_addr, 16);
                if(err_code == NRF_ERROR_INVALID_STATE)
                {
                    is_need_read_flash = false;
                    LOG_PROC("Stop","stop");
                    
                    return;
                }
            }
//            while(user_ble_device_manage_cmd_rsp_send(&m_device_manager,(uint8_t *) fs_config.p_start_addr, 16))
//            {
//                LOG_ERROR("%d");
//            }
        }
        }
#endif
        is_need_read_flash = false;

    }
    if(is_need_write_flash)
    {
        is_need_acquire_temp = true;
        is_need_write_flash = false;
#if 0
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
        fs_call_back_flag = true;
        fs_store(&fs_config, fs_config.p_start_addr + current_store_flash_point, &current_pos,1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
        LOG_INFO("write success");
        current_pos++;
        is_need_write_flash = false;
        
#endif
    //is_need_acquire_temp = true;
    }

    if (is_need_acquire_temp)
    {
        static MyDHT11 value;
        static uint8_t count = 0;

#ifdef USER_USE_DHT11_SENSOR
        while(start_to_read(&value) != NRF_SUCCESS)
        {
            nrf_delay_ms(200);
        }
#else
        int32_t temp = 0;
        sd_temp_get(&temp);
        value.temperatureH =(uint8_t)temp/4;
        value.humidityH = 0;
#endif

        is_need_acquire_temp = false;
        if (count)
        {
            user_flash_struct.temperture1 = value.temperatureH;
            user_flash_struct.humidity1 = value.humidityH;
            LOG_PROC("WRITE","Write flash");
            //store_temp_humity_to_flash();
        }
        else
        {
            user_flash_struct.temperture0 = value.temperatureH;
            user_flash_struct.humidity0 = value.humidityH;
            user_flash_struct.time_stamp = current_time_stamp;
        }

        LOG_PROC("INFO", "%u:HUMIDITY:%d.%d, TEMPERTURE:%d.%d",LOG_UINT(current_time_stamp),LOG_UINT(value.humidityH), LOG_UINT(value.humidityL),
        LOG_UINT(value.temperatureH),LOG_UINT(value.temperatureL));
        user_store_to_flash(&user_flash_struct);

        user_ble_temp_humidity_update(&m_device_manager,value.temperatureH,value.humidityH);
        
        count = ~count;
        
    }
    if(is_need_read)
    {
        //find_current_read_pos();
        static int flag = 0;
        if(flag == 0)
        {
            user_storage_set_address(0x7fff0);
            flag++;
        }

        user_store_to_flash(&user_flash_struct);
        user_flash_struct.time_stamp+=1;
        is_need_read = false;
    }
}