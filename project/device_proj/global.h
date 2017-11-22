/*
 * @File global.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-01 15:59:43 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-07 16:47:55
 */

 #ifndef GLOBAL_H
 #define GLOBAL_H

 #include <stdio.h>
 #include <stdbool.h>
 #include "user_ble_device_manages.h"


 #define USER_PIN_LED 29
 #define USER_PIN_BEEP_1 7
 #define USER_PIN_BEEP_2 8
 #define USER_PIN_BATTERY 5
 #define LIS3DH_INT1_PIN 11
 #define LIS3DH_INT2_PIN 12

 #define CONVERT_MAX_VALUE     580
 #define BATTERY_INITIAL_VALUE 33
 

 typedef struct cmd_struct
 {
     uint8_t command;
     uint8_t params[20];
     uint8_t params_length;
     bool is_need_deal;
 }command_info_t;

 
extern uint32_t current_time_stamp;
extern bool  is_need_acquire_temp;
extern uint32_t current_store_flash_point;
extern user_ble_device_manage_t m_device_manager;

extern uint32_t test_information_count;

extern command_info_t command_info;

extern bool is_need_trans_temp_info;

extern bool is_device_registered;

extern bool is_ble_connected;

extern bool is_need_trun_on_led;

extern bool is_need_request_time_stamp;

extern bool is_ble_connected_event_come;

extern bool is_ble_disconnected_event_come;

extern bool is_beep_stopped;

extern uint32_t request_info_count;
extern bool is_lis3dh_int_come;



 #endif
