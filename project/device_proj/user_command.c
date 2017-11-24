/*
 * @File user_command.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-03 16:29:23 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-07 20:37:13
 */

#include "user_command.h"
#include "nrf.h"
#include "user_log.h"
#include "nrf_gpio.h"
#include "user_ble_device_manages.h"
#include "global.h"
#include "user_storage2.h"
#include "user_app.h"


static void turn_on_led()
{
//    nrf_gpio_pin_clear(USER_PIN_LED);
//    nrf_gpio_pin_clear(20);
    timers_led_start();
}
static void turn_off_led()
{
//    nrf_gpio_pin_set(USER_PIN_LED);
//    nrf_gpio_pin_set(20);
    timers_led_stop();
}

static void start_beep()
{
    //LOG_INFO("Start beep");
    timers_beep_start();

}

static void stop_beep()
{
    timers_beep_stop();
    is_beep_stopped = true;

}


uint32_t deal_width_command(uint8_t command, uint8_t * param, uint16_t len)
{
    if (param == NULL || len > 20)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    user_command_t cmd = command;
    uint8_t *data = param;
    static uint8_t resp_data[20];
#if 0
    LOG_INFO("Received data is:");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", param[i]);
    }
    LOG_INFO("Over");
#endif

    if (cmd >= E_CMD_MAX)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    
    if (cmd == E_CMD_REGISTER)
    {
        /** <Register> */
        resp_data[0] = E_CMD_REGISTER;
        if (user_storage2_is_device_registered())
        {
            resp_data[1] = 0x00;
        }
        else
        {
            user_storage2_register_device();
            resp_data[1] = 0x01;
            is_device_registered = true;
            timers_start();
        }
        user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, 2);
    }
    else if(cmd == E_CMD_WRITE_INFORMATION)
    {
        /** <Write information> */
        //resp_data[0] = E_CMD_WRITE_INFORMATION;
        //memcpy(resp_data+1, frame + 1, len);
        //user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, len);
    }
    else if(cmd == E_CMD_CONTROL)
    {
        /** <Control> */
        resp_data[0] = E_CMD_CONTROL;
        switch (*data)
        {
            case E_FUNCTION_TURN_ON_LED:
                turn_on_led();
                resp_data[1] = E_FUNCTION_TURN_ON_LED;
                break;
            case E_FUNCTION_TURN_OFF_LED:
                turn_off_led();
                resp_data[1] = E_FUNCTION_TURN_OFF_LED;
                user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, 2);
                break;
            case E_FUNCTION_TURN_ON_BEEP:
                start_beep();
                resp_data[1] = E_FUNCTION_TURN_ON_BEEP;
                break;
            case E_FUNCTION_TURN_OFF_BEEP:
                stop_beep();
                resp_data[1] = E_FUNCTION_TURN_OFF_BEEP;
                break;
            default:
                resp_data[1] = E_FUNCTION_MIN;
        }
        user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, 2);
    }
    else if(cmd == E_CMD_TIME_STAP_RESP)
    {
        /** <Updata time stamp> */
        uint32_t *p_time = (uint32_t *)param; 
        //LOG_INFO("%08x", *p_time);
        //LOG_INFO("%08x", current_time_stamp);
        current_time_stamp = *(p_time);
        timers_time_stamp_request_stop();
        //user_ble_device_manage_cmd_rsp_send(&m_device_manager,&current_time_stamp, 2);
    }
    else if(cmd == E_CMD_GET_TEMP_HUMITY)
    {
        is_need_trans_temp_info = true;
    }
    else if(cmd == E_CMD_UNREGISTER)
    {
        LOG_INFO("UNREGISTER");
        user_storage2_unregister_device();
        is_device_registered = false;
        resp_data[0] = E_CMD_UNREGISTER;
        resp_data[1] = 1;
        user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, 2);
        timers_stop();
    }
    else if(cmd == E_CMD_REQUIRE_REGISTER)
    {
        resp_data[0] = E_CMD_REQUIRE_REGISTER;
        if(is_device_registered)
        {
            resp_data[1] = 1;
        }
        else
        {
            resp_data[1] = 0;
        }
        user_ble_device_manage_cmd_rsp_send(&m_device_manager,&resp_data, 2);
    }
    else
    {
        
        //LOG_INFO("SELF DISCONNECT");
        //disconnect_current_connection();
        is_auth_success = true;
        /*
        resp_data[0]  = 0;
        user_ble_device_manage_cmd_rsp_send(&m_device_manager, &resp_data, 0);
        */
    }
    return NRF_SUCCESS;
}