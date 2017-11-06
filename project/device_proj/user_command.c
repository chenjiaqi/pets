/*
 * @File user_command.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-03 16:29:23 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-03 17:35:50
 */

#include "user_command.h"
#include "nrf.h"
#include "user_log.h"
#include "nrf_gpio.h"
#include "user_ble_device_manages.h"

static void turn_on_led()
{
    nrf_gpio_pin_clear(USER_PIN_LED);
    nrf_gpio_pin_clear(20);
}
static void turn_off_led()
{
    nrf_gpio_pin_set(USER_PIN_LED);
    nrf_gpio_pin_set(20);
}

static void start_beep()
{

}

static void stop_beep()
{

}

uint32_t deal_width_command(uint8_t * frame, uint16_t len)
{
    if (frame == NULL || len > 20)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    user_command_t cmd = frame[0];
    uint8_t *data = frame + 1;
    static uint8_t resp_data[20];
    LOG_INFO("Received data is:");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", frame[i]);
    }
    LOG_INFO("Over");
    

    if (cmd >= E_CMD_MAX)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    
    if (cmd == E_CMD_REGISTER)
    {
        /** <Register> */
    }
    else if(cmd == E_CMD_WRITE_INFORMATION)
    {
        /** <Write information> */
       
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
        uint32_t *p_time = (uint32_t *)(frame +1); 
        LOG_INFO("%08x", *p_time);
        LOG_INFO("%08x", current_time_stamp);
        current_time_stamp = *(p_time);
    }
    else
    {
    }
    return NRF_SUCCESS;
}