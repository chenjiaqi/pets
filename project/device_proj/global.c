/*
 * @File global.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-01 15:59:27 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-08 09:50:18
 */

#include "global.h"
#include <stdbool.h>


uint32_t current_time_stamp = 1509465600;
bool is_need_trans_temp_info = false;
bool is_need_acquire_temp = false;
bool is_need_turn_on_led = false;
user_ble_device_manage_t m_device_manager;
bool is_device_registered = false;

bool is_need_request_time_stamp = false;

bool is_ble_connected_event_come = false;

bool is_ble_disconnected_event_come = false;

bool is_lis3dh_int_come = false;

bool is_beep_stopped = true;


bool is_ble_connected = false;

uint32_t current_store_flash_point = 0;

uint32_t test_information_count = 0;

uint32_t app_adv_timeout_in_seconds = 10;

command_info_t command_info = {
    .is_need_deal = false
};

uint32_t request_info_count = 128; // request count

 
 

