#include "user_ble_device_manages.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "user_log.h"
#include "user_ble_srv_common.h"


static uint32_t device_manage_temperature_char_add(user_ble_device_manage_t *p_device_manage,
    const user_ble_device_manage_init_t* p_device_manage_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_sccd_md = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, USER_BLE_UUID_DEVICE_TEMPERATURE);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));


    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t);
    attr_char_value.p_value = 0;

    ble_gatts_char_handles_t handle;
    return sd_ble_gatts_characteristic_add(p_device_manage->service_handle,
                                            &char_md,
                                            &attr_char_value,
                                            &handle);
}


uint32_t user_ble_device_manage_init(user_ble_device_manage_t *p_device_manage,
                                const user_ble_device_manage_init_t* p_device_manage_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;
    LOG_PROC("DEBUG","user_ble_device_manage_init");

    // ADD Services
    BLE_UUID_BLE_ASSIGN(ble_uuid,USER_BLE_UUID_DEVICE_MANAGE_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
    &ble_uuid, &(p_device_manage->service_handle));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //Add temperature Characteristic
    err_code = device_manage_temperature_char_add(p_device_manage, p_device_manage_init);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}

