/*
 * @File user_app.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-27 16:52:28 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-27 17:33:06
 */
#ifndef USER_APP_H
#define USER_APP_H

#include "user_ble_device_manages.h"

void user_app_init(void);
extern user_ble_device_manage_t m_device_manager;

void timers_start();
void timers_stop();
void timers_led_start();
void timers_led_stop();
void timers_beep_start();
void timers_beep_stop();
void user_app_update_device_name(uint8_t temp, uint8_t humidity);
void timers_time_stamp_request_stop();
void timers_time_stamp_request_start();

#endif //  SERVICES_H
