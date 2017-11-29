/*
 * @File user_commmand2.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-28 15:52:32 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-29 11:16:25
 */
#ifndef USER_COMMAND_2_H
#define USER_COMMAND_2_h
#include "stdint.h"
#include "messages.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "druid_frame.h"
#include <stdint.h>

void user_cmd_deal_with_process(const druid_frame_t *frame, druid_frame_t *resp_frame);

uint16_t user_cmd_create_resp_package(CommandResp cmd_resp, uint8_t *p_dest);
#endif
