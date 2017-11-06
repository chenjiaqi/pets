/*
 * @File user_command.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-03 16:28:56 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-06 17:44:05
 */
#include "global.h"

typedef enum e_user_command
{
    E_CMD_MIN = 0,
    E_CMD_REGISTER, //1
    E_CMD_WRITE_INFORMATION, //2
    E_CMD_CONTROL, //3
    E_CMD_TIME_STAP_RESP, //4
    E_CMD_MAX //5
}user_command_t;

typedef enum e_control_function
{
    E_FUNCTION_MIN = 0,
    E_FUNCTION_TURN_ON_LED,
    E_FUNCTION_TURN_OFF_LED,
    E_FUNCTION_TURN_ON_BEEP,
    E_FUNCTION_TURN_OFF_BEEP,
    E_FUNCTION_MAX
}user_control_function_t;


uint32_t deal_width_command(uint8_t * frame, uint16_t len);