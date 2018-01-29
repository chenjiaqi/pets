/*
 * @File undefined 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-30 17:11:54 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-29 13:48:59
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
//#include "user_storage.h"
#include "user_storage2.h"
#include "ble_advertising.h"
#include "global.h"
#include "nrf_gpio.h"
#include "nrf_drv_saadc.h"
#include "frame_queue.h"
#include "druid_frame.h"
#include "user_command_2.h"
#include "user_app.h"
/**@brief Function for User Process
 *
 * @details Deal with user process
 *
 */
extern bool is_need_write_flash ;
extern bool is_need_read_flash ;



static bool fs_call_back_flag = false;


static user_flash_structure_t user_flash_struct;

extern bool is_need_turn_on_led;





extern void power_manage();
extern bool is_need_read;
extern void advertising_init();

uint8_t get_battery_level(int16_t value)
{
    if (value <= 0)
    {
        return 0;
    }
    return (value / ((CONVERT_MAX_VALUE / BATTERY_INITIAL_VALUE + 1))) + 1;
}
extern void sleep_mode_enter();

static void deal_with_frame_from_queue()
{
    static uint8_t recv_frame[MAX_FRAME_SIZE];
    uint16_t len = 0;
    if(frame_queue_is_empty())
    {
        return;
    }
    len = frame_queue_get(recv_frame);

    /*if(druid_frame_is_frame_valid(recv_frame, len))
    {
        LOG_INFO("valid");
    }
    else{
        LOG_INFO("Invalid");
    }*/
    druid_frame_t *p_druid_frame = druid_frame_get_frame_info(recv_frame, len);
    
    if(p_druid_frame)
    {
        druid_frame_t resp_frame = {0};
        uint8_t buf[256];
        resp_frame.p_data = buf;
        user_cmd_deal_with_process(p_druid_frame, &resp_frame);
        
        uint8_t len = 0;
        //len = user_cmd_create_resp_package(CommandResp_E_CMD_RESP_GET_DEVICE_INFO, buf);
        //len = user_cmd_create_cmd_package(CommandResp_E_CMD_RESP_AUTH, buf);
        //len = user_cmd_create_cmd_package(Command_E_CMD_LED, buf);


        druid_frame_t frame;
        frame.seq = 0x01;
        frame.cmd = 0x02;
        frame.len = len;
        frame.p_data = buf;
        //druid_set_construct_trans_frame(frame);
        druid_set_construct_trans_frame(resp_frame);
        
        uint8_t split_length = 0;
        uint8_t *p_split_frame = NULL;
        do{
            //printf("\r\n");
            p_split_frame = get_split_frame(&split_length);
            if(p_split_frame)
            {
                uint32_t ret_code;
                //ret_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager, p_split_frame, split_length);
                do{
                ret_code = user_ble_device_manage_cmd_rsp_send(&m_device_manager, p_split_frame, split_length);
            }while((ret_code !=NRF_SUCCESS) && (ret_code != NRF_ERROR_INVALID_STATE));
                /*for(int i = 0; i <split_length; i++)
                {
                    printf("%02x ", p_split_frame[i]);
                }
                */
            }
        }while(p_split_frame);
        //printf("send over\r\n");
        /*
        LOG_INFO(" ");
        for (int i = 0; i < len; i++)
        {
            printf("%02X ", buf[i]);
        }
        LOG_INFO(" ");
        LOG_INFO("resp_package len is : %d", len);
        */
    }
    else
    {
        LOG_INFO("invalid");
    }

}

void user_process(void)
{
    deal_with_frame_from_queue();

    if(is_need_read_flash)
    {
        if (user_storage2_is_device_registered())
        {
            //LOG_INFO("REGISTERED");
        }
        else{
            //LOG_INFO(" NOT REGISTERED");
        }
        //user_storage2_test_set_read_addr();
        uint32_t count = user_storage2_get_record_count();

        is_need_read_flash = false;

    }
    if(is_need_write_flash)
    {
        is_need_acquire_temp = true;
        is_need_write_flash = false;
    }

    if (is_need_acquire_temp)
    {
        static MyDHT11 value;
        static uint8_t count = 0;

        int32_t temp = 0;
        sd_temp_get(&temp);
        value.temperatureH =(uint8_t)temp/4;
        value.humidityH = 0;

        is_need_acquire_temp = false;
        if (count % 2)
        {
            user_flash_struct.temperture1 = value.temperatureH;
            user_flash_struct.humidity1 = value.humidityH;
            LOG_PROC("WRITE","Write flash");
            user_storage2_store_a_record(&user_flash_struct);
        }
        else
        {
            user_flash_struct.temperture0 = value.temperatureH;
            user_flash_struct.humidity0 = value.humidityH;
            user_flash_struct.time_stamp = current_time_stamp;
        }

        static int16_t battery_level;
        static int16_t battery_level2;
        static int16_t battery_level3;
        static uint8_t battery;

        if(count % 2 == 0)
        {
            bool ret = false;
            nrf_gpio_pin_clear(6);
            nrf_delay_ms(300);
            nrf_drv_saadc_sample_convert(NRF_SAADC_INPUT_AIN3 ,&battery_level);
            nrf_delay_ms(30);
            nrf_drv_saadc_sample_convert(NRF_SAADC_INPUT_AIN3 ,&battery_level2);
            nrf_delay_ms(30);
            nrf_drv_saadc_sample_convert(NRF_SAADC_INPUT_AIN3 ,&battery_level3);
            nrf_gpio_pin_set(6);
        }

        LOG_PROC("INFO", "%u:HUMIDITY:%d.%d, TEMPERTURE:%d.%d, %d",LOG_UINT(current_time_stamp),LOG_UINT(value.humidityH), LOG_UINT(value.humidityL),
        LOG_UINT(value.temperatureH),LOG_UINT(value.temperatureL), battery_level + battery_level2 + battery_level3);


        user_ble_temp_humidity_update(&m_device_manager,value.temperatureH,battery_level + battery_level2 + battery_level3);
        battery = get_battery_level((battery_level+battery_level2+battery_level3)/3);
        LOG_PROC("INFO","current berrery is %d", battery);
        user_app_update_device_name(value.temperatureH, value.humidityH, battery);

        count ++;
        LOG_PROC("DEV","%d", count);
        if(count >=64)
        {
            count = 1;
        }
        
    }

    if(is_need_read)
    {
        //find_current_read_pos();
        static int flag = 0;
        if(flag == 0)
        {
            //user_storage_set_address(0x7fff0);
            flag++;
        }
        //user_store_to_flash(&user_flash_struct);
        user_flash_struct.time_stamp+=1;
        is_need_read = false;
    }

    if (is_need_trans_temp_info)
    {
        if(is_ble_connected)
        {
            static uint16_t count = 0;
            uint32_t *p = user_storage2_get_a_record();
            static uint8_t temp_data[20];
            temp_data[0] = 0x05;
            memcpy(temp_data+1, (uint8_t *)p, 16);
            if (p && request_info_count > 0)
            {
                count++;
                //user_ble_device_manage_cmd_rsp_send(&m_device_manager, temp_data, 17);
                uint32_t err_code = 0;
                do
                {
                    err_code =  user_ble_device_manage_cmd_rsp_send(&m_device_manager, temp_data, 17);
                }while((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE));

                request_info_count --;
            }
            else
            {
                LOG_INFO("Trans info %d", count);
                is_need_trans_temp_info = false;
                count = 0;
                request_info_count = 128;
                // trans over
                // write current pos into flash
            }
        }
        else
        {
            is_need_trans_temp_info = false;
            // disconnected when in trans
            // write current pos into flash
        }
    }

    if (is_need_turn_on_led)
    {
        //LOG_INFO("turn on led");
        nrf_gpio_pin_clear(USER_PIN_LED);
        nrf_gpio_pin_clear(20);
        nrf_delay_ms(5);
        nrf_gpio_pin_set(USER_PIN_LED);
        nrf_gpio_pin_set(20);
        is_need_turn_on_led = false;
    }

    if (is_need_request_time_stamp)
    {
        //LOG_INFO("Request time stamp");
        uint8_t time_stamp_request_frame[] = {0x04};
        user_ble_device_manage_cmd_rsp_send(&m_device_manager,time_stamp_request_frame,1);
        is_need_request_time_stamp = false;
    }

    if (is_ble_connected_event_come)
    {
        //timers_time_stamp_request_start();
        is_auth_success = false;
        //is_current_connection_registered = false;
        //app_timer_start(m_auth_timer_id, AUTH_MEAS_INTERVAL, NULL);
        //timer_auth_start();
        is_ble_connected_event_come = false;
        timers_led_stop();
    }

    if (is_ble_disconnected_event_come)
    {
        timers_time_stamp_request_stop();
        timers_led_stop();
        timers_beep_stop();
        nrf_gpio_pin_set(USER_PIN_BEEP_1);
        nrf_gpio_pin_set(USER_PIN_BEEP_2);
        if (is_device_registered == false)
        {
            LOG_INFO("should enter sleep");
            sd_power_system_off();
        }

        if(is_current_connection_registered)
        {
            user_advertising_init();
            is_current_connection_registered = false;
            LOG_INFO("restart device");
            APP_ERROR_CHECK(1);
        }
        is_ble_disconnected_event_come = false;
    }

    if (is_beep_stopped)
    {
        nrf_gpio_pin_set(USER_PIN_BEEP_1);
        nrf_gpio_pin_set(USER_PIN_BEEP_2);
        is_beep_stopped = false;
    }
}