/*
 * @File user_ble.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-25 14:39:55 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-25 14:54:29
 */
#ifndef USER_BLE_H
#define USER_BLE_H

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void user_ble_gap_init(void);
void user_ble_service_init(void);

#endif // USER_BLE_H