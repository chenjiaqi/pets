/*
 * @File user_ble.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-25 14:45:54 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-26 19:04:15
 */
#include "user_ble.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "user_log.h"

#define DEVICE_NAME                     "Nordic_CCCCCCC"                                /**< Name of device. Will be included in the advertising data. */
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */



/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
void user_ble_gap_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void user_ble_service_init(void)
{
    LOG_PROC("Func",__func__);
    uint32_t        error_code;
    ble_uuid_t cus_uuid;
    cus_uuid.type = BLE_UUID_TYPE_BLE;
    cus_uuid.uuid = 0x7777;
    error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
    &cus_uuid, NULL); 
    error_code = NRF_SUCCESS;
    APP_ERROR_CHECK(error_code);
    /** <add a characteristic> */
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&attr_md, 0, sizeof(attr_md));
    //BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_BATTERY_LEVEL_CHAR);
    BLE_UUID_BLE_ASSIGN(ble_uuid, 0x7778);
    ble_gap_conn_sec_mode_t read_perm;       /**< Read permissions. */
    ble_gap_conn_sec_mode_t write_perm;       /**< Read permissions. */

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

//    attr_md.read_perm  =  read_perm;
//    attr_md.write_perm =  write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    uint8_t initial_value;
    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);
    attr_char_value.p_value   = &initial_value;
    //ble_gatts_attr_md_t cccd_md;
    ble_gatts_char_handles_t handle;
    error_code = sd_ble_gatts_characteristic_add(0,&char_md, &attr_char_value,&handle);
    LOG_PROC("ERRORCODE", "%u",(unsigned int)error_code);
    //UNUSED(NRF_SUCCESS);
}