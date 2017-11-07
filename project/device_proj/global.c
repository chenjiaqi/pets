/*
 * @File global.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-01 15:59:27 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-07 17:29:44
 */

 #include "global.h"
 #include <stdbool.h>


uint32_t current_time_stamp = 1509465600;
bool is_need_trans_temp_info = true;
bool is_need_acquire_temp = false;
user_ble_device_manage_t m_device_manager;
bool is_device_registered = false;

uint32_t current_store_flash_point = 0;

uint32_t test_information_count = 0;
 
 

