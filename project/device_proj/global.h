/*
 * @File global.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-01 15:59:43 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-03 09:51:26
 */

 #ifndef GLOBAL_H
 #define GLOBAL_H

 #include <stdio.h>
 #include <stdbool.h>
 #include "user_ble_device_manages.h"


 #define USER_PIN_LED 29

 
extern uint32_t current_time_stamp;
extern bool  is_need_acquire_temp;
extern uint32_t current_store_flash_point;
extern user_ble_device_manage_t m_device_manager;


 #endif
