/*
 * @File user_ble_device_manages.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-26 17:07:27 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-26 18:06:03
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
    uint16_t                    conn_handle;
};

uint32_t user_ble_device_manage_init(user_ble_device_manage_t *p_device_manage,
                                    const user_ble_device_manage_init_t* p_manage);
#endif
