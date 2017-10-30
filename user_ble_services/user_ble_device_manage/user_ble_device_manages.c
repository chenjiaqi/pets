#include "user_ble_device_manages.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "user_log.h"
#include "user_ble_srv_common.h"

static void on_write(user_ble_device_manage_t *p_device_manage, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    p_device_manage->data_handler(p_device_manage, p_evt_write->data, p_evt_write->len);
    LOG_PROC("onwrite","%d", (p_evt_write->data)[0]);
}

static void on_connect(user_ble_device_manage_t *p_device_manage, ble_evt_t *p_ble_evt)
{
    p_device_manage->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(user_ble_device_manage_t * p_device_manage, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_device_manage->conn_handle = BLE_CONN_HANDLE_INVALID;
}


static uint32_t device_manage_temperature_char_add(user_ble_device_manage_t *p_device_manage,
    const user_ble_device_manage_init_t* p_device_manage_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t init_value_encode[1];
    memset(&char_md, 0, sizeof(char_md));
    init_value_encode[0] = 15;

    char_md.char_props.read = 1;
//    char_md.char_props.write = 1;
    char_md.p_char_user_desc = NULL; 
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_sccd_md = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, USER_BLE_UUID_DEVICE_TEMPERATURE_CHAR);

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
    attr_char_value.p_value = (uint8_t *)init_value_encode;

    return sd_ble_gatts_characteristic_add(p_device_manage->service_handle,
                                            &char_md,
                                            &attr_char_value,
                                            &p_device_manage->temperature_handle);
}

static uint32_t device_manage_humidity_char_add(user_ble_device_manage_t *p_device_manage,
    const user_ble_device_manage_init_t* p_device_manage_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t init_value_encode[1];
    memset(&char_md, 0, sizeof(char_md));
    init_value_encode[0] = 15;

    char_md.char_props.read = 1;
//    char_md.char_props.write = 1;
    char_md.p_char_user_desc = NULL; 
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_sccd_md = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, USER_BLE_UUID_DEVICE_HUMIDITY_LEVER_CHAR);

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
    attr_char_value.p_value = (uint8_t *)init_value_encode;

    return sd_ble_gatts_characteristic_add(p_device_manage->service_handle,
                                            &char_md,
                                            &attr_char_value,
                                            &p_device_manage->humidity_level_handle);
}

static uint32_t device_manage_led_char_add(user_ble_device_manage_t *p_device_manage,
    const user_ble_device_manage_init_t* p_device_manage_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t init_value_encode[1];
    memset(&char_md, 0, sizeof(char_md));
    init_value_encode[0] = 15;

    //char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    char_md.p_char_user_desc = NULL; 
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_sccd_md = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, USER_BLE_UUID_DEVICE_LED_CHAR);

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
    attr_char_value.p_value = (uint8_t *)init_value_encode;

    return sd_ble_gatts_characteristic_add(p_device_manage->service_handle,
                                            &char_md,
                                            &attr_char_value,
                                            &p_device_manage->led_handle);
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

    err_code = device_manage_humidity_char_add(p_device_manage, p_device_manage_init);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = device_manage_led_char_add(p_device_manage, p_device_manage_init);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}

void user_ble_device_manage_on_ble_event(user_ble_device_manage_t *p_dev_manage,ble_evt_t * p_ble_evt)
{
    if (p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            LOG_PROC("EVENT","BLE_GAP_EVT_CONNECTED");
            on_connect(p_dev_manage,p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            LOG_PROC("EVENT","BLE_GAP_EVT_CONNECTED");
            on_disconnect(p_dev_manage,p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            LOG_PROC("EVENT","BLE_GATTS_EVT_WRITE");
            LOG_PROC("EVENT","%d", p_ble_evt->evt.gatts_evt.params.write.handle);
            on_write(p_dev_manage, p_ble_evt);
            break;
        case BLE_GATTS_EVT_HVC:
            LOG_PROC("EVENT","HVC");
            break;
        default:
            break;
    }
}

uint32_t user_ble_temp_humidity_update(user_ble_device_manage_t *p_device_manage, uint8_t temp, uint8_t humidity)
{
    if(p_device_manage == NULL)
    {
        return NRF_ERROR_NULL;
    }
    LOG_PROC("UPDATE","%d",temp);
    LOG_PROC("UPDATE","%d",humidity);

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;
    memset(&gatts_value, 0, sizeof(gatts_value));
    gatts_value.len = sizeof(uint8_t);
    gatts_value.offset = 0;
    gatts_value.p_value = &temp;
    
    err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, 
                                    p_device_manage->temperature_handle.value_handle,
                                    &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    gatts_value.p_value = &humidity;
    err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, 
                                    p_device_manage->humidity_level_handle.value_handle,
                                    &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return err_code;

}
