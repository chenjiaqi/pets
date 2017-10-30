/*
 * @File user_ble_sec_common.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-26 17:19:42 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-30 19:15:48
 */
#ifndef USER_BLE_SRV_COMMON_H
#define USER_BLE_SRV_COMMON_H

#include "ble_gatt.h"

#define BLE_DEVICE_MANAGE_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */


/** @defgroup USER_UUID_SERVICES Service UUID definitions
 * @{ */
#define USER_BLE_UUID_DEVICE_MANAGE_SERVICE 0x7711
/** @} */



/** @defgroup USER_UUID_CHARACTERISTICS UUID definitions
 * @{ */
#define USER_BLE_UUID_DEVICE_TEMPERATURE_CHAR 0x7712
#define USER_BLE_UUID_DEVICE_HUMIDITY_LEVER_CHAR 0x7713
#define USER_BLE_UUID_DEVICE_LED_CHAR 0x7714
#define USER_BLE_UUID_DEVICE_BEEP_CHAR 0x7715
#define USER_BLE_UUID_DEVICE_CMD_RSP_CHAR 0x7716

/** @} */
#endif

