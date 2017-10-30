/*
 * @File user_ble_device_manages.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-26 17:07:27 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-29 12:23:53
 */
#ifndef USER_BLE_DEVICE_MANAGES_H
#define USER_BLE_DEVICE_MANAGES_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/**@brief Function for initializing the Device Manage Service
 *
 * @details 
 *
 * @ param[in]
 *
 * @retval
 *
 */

typedef struct user_ble_device_manage_s user_ble_device_manage_t;
//typedef void (*user_ble_device_manage_evt_handler_t) (user_ble_device_manage_t* p_device_manage, user_ble_device_manage_init_t* p_device_manage_init);
typedef void (*user_ble_device_manage_data_handler_t) (user_ble_device_manage_t *p_device_manage, uint8_t *p_data, uint16_t length);


typedef struct 
{
    int initial;
}user_ble_device_manage_init_t;

struct user_ble_device_manage_s
{
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    temperature_handle;
    ble_gatts_char_handles_t    humidity_level_handle;
    ble_gatts_char_handles_t    time_stamp;
    ble_gatts_char_handles_t    led_handle;
    uint16_t                    conn_handle;
    user_ble_device_manage_data_handler_t data_handler;
};

uint32_t user_ble_device_manage_init(user_ble_device_manage_t *p_device_manage,
                                    const user_ble_device_manage_init_t* p_manage);

void user_ble_device_manage_on_ble_event(user_ble_device_manage_t *p_dev_manage, ble_evt_t * p_ble_evt);

uint32_t user_ble_temp_humidity_update(user_ble_device_manage_t *p_device_manage, uint8_t temp, uint8_t humidity);
#endif
