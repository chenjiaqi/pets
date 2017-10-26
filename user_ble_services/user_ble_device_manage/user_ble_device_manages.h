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


typedef struct 
{
    int initial;
}user_ble_device_manage_init_t;


struct user_ble_device_manage_s
{
    uint16_t service_handle;
};


uint32_t user_ble_device_manage_init(user_ble_device_manage_t *p_device_manage,
                                    const user_ble_device_manage_init_t* p_manage);
#endif
