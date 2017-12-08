/*
 * @File user_command_2.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-28 15:52:39 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-29 16:48:47
 */
#include "user_command_2.h"
#include "user_log.h"
#include "user_app.h"
#include "global.h"
#include "user_process.h"

static bool is_operate_success = false;


void user_cmd_deal_with_process(const druid_frame_t *p_frame, druid_frame_t *p_resp_frame)
{
    if(!(p_frame && p_resp_frame))
    {
        return;
    }
    
    /** <decode command> */

    bool status;
    
    RequestMessage message = RequestMessage_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(p_frame->p_data,p_frame->len);
    status = pb_decode(&stream, RequestMessage_fields, &message);
    if(!status)
    {
        LOG_INFO("decode failed");
        return;
    }
    LOG_INFO("DECODE");
    
    if (!message.has_cmd)
    {
        return;
    }
    LOG_INFO("Fratype:%d, Cmd is:%d", message.frametype,message.cmd);
    if (message.has_params)
    {
        LOG_INFO("Pam len:%d", message.params.size);
    }
    switch(message.cmd)
    {
        case Command_E_CMD_AUTH:
            LOG_INFO("AUTH");
            if(message.has_params  &&message.params.size == 16)
            {

            }
            break;
        case Command_E_CMD_REGISTER:
            LOG_INFO("REGISTER");
            is_operate_success = false;
            /** <device is unregistered> */
            if (!is_device_registered)
            {
                is_device_registered = user_storage2_register_device();
                if (is_device_registered)
                {
                    is_operate_success= true;
                    timers_start();
                }
            }
            break;
        case Command_E_CMD_UNREGISTER:
            LOG_INFO("UNREGISTER");

            is_operate_success = false;

            /** <device is registered> */
            if(is_device_registered)
            {
                user_storage2_unregister_device();
                is_device_registered = user_storage2_is_device_registered();
                if(!is_device_registered)
                {
                    is_operate_success = true;
                    timers_stop();
                }
            }

            break;
        case Command_E_CMD_LED:
            if(message.has_params && message.params.size == 1)
            {
                if(message.params.bytes[0] == TwoState_E_ON)
                {
                    LOG_INFO("LED ON");
                    timers_led_start();
                }
                else if (message.params.bytes[0] = TwoState_E_ON)
                {
                    timers_led_stop();
                    LOG_INFO("LED OFF");
                }
                else
                {
                    LOG_INFO("PARAM ERROR");
                }
            }
            break;
        case Command_E_CMD_BEEP:
            if (message.has_params && message.params.size == 1)
            {
                if (message.params.bytes[0] == TwoState_E_ON)
                {
                    LOG_INFO("BEEP ON");
                    timers_beep_start();
                }
                else if (message.params.bytes[0] = TwoState_E_ON)
                {
                    LOG_INFO("BEEP OFF");
                    timers_beep_stop();
                }
                else
                {
                }
            }
            break;
        case Command_E_CMD_GET_STORE_DATA:
            LOG_INFO("GET DATA");
            break;
        case Command_E_CMD_GET_DEVICE_INFO:
            LOG_INFO("GET INFO");
            break;
    }

     /** <create resp frame> */
    p_resp_frame->seq = p_frame->seq;
    p_resp_frame->cmd = p_frame->cmd;

    uint16_t param_len = user_cmd_create_resp_package(message.cmd, p_resp_frame->p_data);
    p_resp_frame->len = param_len;



#if 0
    printf("\r\n");
    for(int i = 0; i <p_frame->len; i++)
    {
        printf("%02X ", p_frame->p_data[i]);
    }
    printf("\r\n");
#endif
}

/** <Create resp package, part : proto> */
uint16_t user_cmd_create_resp_package(CommandResp cmd_resp, uint8_t *p_dest)  // part proto
{

    uint16_t length = 0;
    uint16_t params_length = 0;
    uint8_t param_buf[255];

    RequestMessage message = RequestMessage_init_zero;
    pb_ostream_t out_put = pb_ostream_from_buffer(p_dest,255);

    message.frametype = FrameType_E_FRAME_TYPE_RESPONSE;
    message.has_cmd_resp= true;
    message.cmd_resp = cmd_resp;

    RespGetDeviceParams device_info = RespGetDeviceParams_init_zero;
    pb_ostream_t params_out_put = pb_ostream_from_buffer(param_buf, 255);


    user_flash_structure_t *p_record;

    RespGetdataParams datas = RespGetdataParams_init_zero;
    uint32_t record_count = 0;



    switch (cmd_resp)
    {
    case CommandResp_E_CMD_RESP_AUTH:
        message.has_params = true;
        message.params.size = 16;
        message.params.bytes[0] = RespCode_E_RESP_CODE_SUCCESS;
        break;

    case CommandResp_E_CMD_RESP_BEEP:
        message.has_params = true;
        message.params.size = 1;
        message.params.bytes[0] = RespCode_E_RESP_CODE_SUCCESS;
        break;

    case CommandResp_E_CMD_RESP_LED:
        message.has_params = true;
        message.params.size = 1;
        message.params.bytes[0] = RespCode_E_RESP_CODE_SUCCESS;
        break;

    case CommandResp_E_CMD_RESP_GET_DEVICE_INFO:


        device_info.device_id.size = 6;
        user_get_mac_address(device_info.device_id.bytes);
        device_info.has_is_registered = true;
        device_info.is_registered = is_device_registered;

        pb_encode(&params_out_put, RespGetDeviceParams_fields, &device_info);
        params_length = params_out_put.bytes_written;

        message.has_params = true;
        message.params.size = params_length;
        memcpy(message.params.bytes, param_buf, params_length);

        break;

    case CommandResp_E_CMD_RESP_GET_STORE_DATA:
        record_count = user_storage2_get_record_count();
        if (record_count >= 10)
        {
            record_count = 10;
        }
        LOG_INFO("Record count is:%d", record_count);
        datas.info_count = record_count * 2;
        for(int i = 0; i <record_count ; i++)
        {
            p_record = (user_flash_structure_t *)user_storage2_get_a_record();
            if(p_record)
            {
                datas.info[i * 2].timestamp = p_record->time_stamp;
                datas.info[i * 2].has_temperture = true;
                datas.info[i *2].temperture = p_record->temperture0;

                datas.info[i * 2 + 1].timestamp = p_record->time_stamp + 1800;
                datas.info[i * 2 + 1].has_temperture = true;
                datas.info[i *2 + 1].temperture = p_record->temperture1;
            }
        }

        pb_encode(&params_out_put,RespGetdataParams_fields,&datas);
        params_length = params_out_put.bytes_written;
        LOG_INFO("%d",params_out_put.bytes_written);

        message.has_params = true;
        message.params.size = params_length;
        memcpy(message.params.bytes, param_buf, params_length);

        break;

    case CommandResp_E_CMD_RESP_REGISTER:
        message.has_params = true;
        message.params.size = 1;
        if (is_operate_success)
        {
            message.params.bytes[0] = RespCode_E_RESP_CODE_SUCCESS;
        }
        else
        {
            message.params.bytes[0] = RespCode_E_RESP_CODE_FAILED;
        }
        break;

    case CommandResp_E_CMD_RESP_UNREGISTER:
        message.has_params = true;
        message.params.size = 1;
        if (is_operate_success)
        {
            message.params.bytes[0] = RespCode_E_RESP_CODE_SUCCESS;
        }
        else
        {
            message.params.bytes[0] = RespCode_E_RESP_CODE_FAILED;
        }
        break;
    }
    pb_encode(&out_put, RequestMessage_fields, &message);
    LOG_INFO("length is %d\r\n", out_put.bytes_written);
    length = out_put.bytes_written;
    return length;
}

uint16_t user_cmd_create_cmd_package(Command cmd, uint8_t *p_dest)
{

    uint8_t *params_buffer[128];
    uint16_t length = 0;
    RequestMessage message = RequestMessage_init_zero;

    message.frametype = FrameType_E_FRAME_TYPE_REQUEST;
    message.has_cmd = true;
    message.cmd = cmd;

    pb_ostream_t out_put = pb_ostream_from_buffer(p_dest,255);

    switch (cmd)
    {
    case Command_E_CMD_AUTH:
        message.has_params = true;
        message.params.size = 16;
        break;
    case Command_E_CMD_BEEP:
        message.has_params = true;
        message.params.size = 1;
        message.params.bytes[0] = TwoState_E_ON;
        break;
    case Command_E_CMD_LED:
        message.has_params = true;
        message.params.size = 1;
        message.params.bytes[0] = TwoState_E_ON;
        break;
    case Command_E_CMD_REGISTER:
        break;
    case Command_E_CMD_UNREGISTER:
        break;
    case Command_E_CMD_GET_DEVICE_INFO:
        break;
    case Command_E_CMD_GET_STORE_DATA:
        break;
    default:
        break;
    }
    pb_encode(&out_put, RequestMessage_fields, &message);
    length = out_put.bytes_written;
    return length;
}