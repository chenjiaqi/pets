/*
 * @File services.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-27 16:53:45 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-06 17:51:56
 */
#include "user_app.h"
#include "bsp.h"
#include "app_uart.h"
#include "nrf_drv_gpiote.h"
#include "ble_gatt.h"
#include "bsp.h"
#include "app_util_platform.h"
#include "nrf.h"
#include "nordic_common.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "bsp_btn_ble.h"
#include "user_ble_device_manages.h"
#include "user_log.h"
#include "user_ble_srv_common.h"
#include "app_timer.h"
#include "user_device_info.h"
#include "fstorage.h"
#include "global.h"
#include <stdbool.h>
#include "nrf_temp.h"
#include "user_command.h"
#include "user_storage2.h"
#include "nrf_drv_saadc.h"
#include "global.h"

//user_ble_device_manage_t m_device_manager;

APP_TIMER_DEF(m_temp_acq_timer_id);
APP_TIMER_DEF(m_led_timer_id);
APP_TIMER_DEF(m_beep_timer_id);
APP_TIMER_DEF(m_request_time_stamp_timer_id);




#define DEVICE_NAME_PREX                     "DRUID_"                               /**< Name of device. Will be included in the advertising data. */
#define DEVICE_NAME                     "DRUID_AAAAAAAAAAAA_1F4021"                               /**< Name of device. Will be included in the advertising data. */
#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif
#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */
#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */
#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
#define APP_ADV_INTERVAL                1600                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                         /**< The advertising timeout (in units of seconds). */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#ifdef DEBUG_MODE
#define TEMPERTURE_ACQUISITION_MEAS_INTERVAL     APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#else
#define TEMPERTURE_ACQUISITION_MEAS_INTERVAL     APP_TIMER_TICKS(1800*1000, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#endif
#define LED_MEAS_INTERVAL     APP_TIMER_TICKS(1500, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#define BEEP_MEAS_INTERVAL     APP_TIMER_TICKS(1, APP_TIMER_PRESCALER) / 5  /**< Battery level measurement interval (ticks). */
#define TIME_STAMP_REQUEST_MEAS_INTERVAL     APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)   /**< Battery level measurement interval (ticks). */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_uuid_t                       m_adv_uuids[] = {{USER_BLE_UUID_DEVICE_MANAGE_SERVICE,0x01}};  /**< Universally unique service identifier. */



static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;
//    LOG_PROC("ADV_EVENT", "EVENT");
    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        APP_ERROR_CHECK(err_code);
        break;
    case BLE_ADV_EVT_IDLE:
        LOG_PROC("SLEEP", "ENTER SLEEP MODE");
        sleep_mode_enter();
        break;
    default:
        break;
    }
}

static void sys_evt_dispatch(uint32_t sys_evt)
{
    fs_sys_event_handler(sys_evt);
}

static void uart_event_handle(app_uart_evt_t *p_event)
{
}
static void on_ble_evt(ble_evt_t *p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
        APP_ERROR_CHECK(err_code);
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        is_ble_connected = true;
        //is_need_request_time_stamp = true;
        is_ble_connected_event_come = true;

        break; // BLE_GAP_EVT_CONNECTED

    case BLE_GAP_EVT_DISCONNECTED:
        err_code = bsp_indication_set(BSP_INDICATE_IDLE);
        APP_ERROR_CHECK(err_code);
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        is_ble_connected = false;
        timers_led_stop();
        timers_beep_stop();
        break; // BLE_GAP_EVT_DISCONNECTED

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        // No system attributes have been stored.
        err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTC_EVT_TIMEOUT

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTS_EVT_TIMEOUT

    case BLE_EVT_USER_MEM_REQUEST:
        err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
        APP_ERROR_CHECK(err_code);
        break; // BLE_EVT_USER_MEM_REQUEST

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
    {
        ble_gatts_evt_rw_authorize_request_t req;
        ble_gatts_rw_authorize_reply_params_t auth_reply;

        req = p_ble_evt->evt.gatts_evt.params.authorize_request;

        if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
        {
            if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
                (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
            {
                if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                {
                    auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                }
                else
                {
                    auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                }
                auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                           &auth_reply);
                APP_ERROR_CHECK(err_code);
            }
        }
    }
    break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
        err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                   NRF_BLE_MAX_MTU_SIZE);
        APP_ERROR_CHECK(err_code);
        break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

    default:
        // No implementation needed.
        break;
    }
}
static void ble_evt_dispatch(ble_evt_t *p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    user_ble_device_manage_on_ble_event(&m_device_manager, p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
}

static void uart_init(void)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
        {
//            RX_PIN_NUMBER,
//            TX_PIN_NUMBER,
            22,
            23,
            RTS_PIN_NUMBER,
            CTS_PIN_NUMBER,
            APP_UART_FLOW_CONTROL_DISABLED,
            false,
            UART_BAUDRATE_BAUDRATE_Baud115200};

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       //NULL,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);
    APP_ERROR_CHECK(err_code);
    //app_uart_close();
}

static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

// Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


static void user_ble_device_manage_data_handler(user_ble_device_manage_t *p_device_manage, uint8_t *p_data, uint16_t length)
{
#if 0
    LOG_PROC("info", "this is call back, %d", p_data[0]);
    if (p_data[0] == 0)
    {
        nrf_gpio_pin_set(LED_4);
        nrf_gpio_pin_set(29);
    }
    else
    {
        nrf_gpio_pin_clear(LED_4);
        nrf_gpio_pin_clear(29);
    }
    user_ble_device_manage_cmd_rsp_send(&m_device_manager, p_data, 1);
#endif
    //deal_width_command(p_data, length);
    command_info.command = p_data[0];
    command_info.is_need_deal = true;
    memcpy(command_info.params, p_data+1, length-1);
    command_info.params_length = length - 1;
}

static void services_init(void)
{
    uint32_t err_code;
    user_ble_device_manage_init_t device_manage_init;

    /** <initial user ble device manage services> */
    memset(&device_manage_init, 0, sizeof(device_manage_init));
    m_device_manager.data_handler = user_ble_device_manage_data_handler;
    err_code = user_ble_device_manage_init(&m_device_manager, &device_manage_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
uint8_t device_name_str[] = DEVICE_NAME;
static void advertising_init(void);
//void user_app_update_device_name(char *temp_humidity)
void user_app_update_device_name(uint8_t temp, uint8_t humidity, uint8_t battery_level)
{
    if (!is_ble_connected)
    {
        uint32_t err_code;
        sprintf((char *)(device_name_str + 19), "%02X%02X%02X", temp, humidity, battery_level);
        LOG_INFO("DEVICE NAMEIS :%s",device_name_str);
        ble_gap_conn_sec_mode_t sec_mode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

        err_code = sd_ble_gap_device_name_set(&sec_mode,
                                              (const uint8_t *)device_name_str,
                                              strlen((char *)device_name_str));
        APP_ERROR_CHECK(err_code);
        advertising_init();
        //LOG_INFO("UPDATE NAME");
    }
}
static void gap_params_init(void)
{
    uint32_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    
    err_code = user_get_mac_address_str((uint8_t *)(device_name_str + strlen(DEVICE_NAME_PREX)));

    APP_ERROR_CHECK(err_code);
    sprintf((char *)(device_name_str + 18), "%s", "_163D");
    //LOG_PROC("NAME", "%s", device_name_str);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)device_name_str,
                                          strlen((char *)device_name_str));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;
    ble_adv_modes_config_t options;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;

    //advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
    uint32_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

extern bool is_need_read;
//extern uint32_t current_time_stamp;
static void temp_acq_timeout_handler(void *p_context)
{
    //LOG_INFO("Temperature acquisition timeout handler");
    current_time_stamp = current_time_stamp + 1800;
    is_need_acquire_temp = true;
}

extern bool is_need_turn_on_led;
static void led_timeout_handler(void *p_context)
{
    is_need_turn_on_led = true;
}

static void beep_timeout_handler(void *p_context)
{
    static uint32_t beep_count = 0;
    if ((beep_count >> 10) % 3 == 0)
    {
        if (beep_count % 2)
        {
            nrf_gpio_pin_clear(USER_PIN_BEEP_1);
            nrf_gpio_pin_set(USER_PIN_BEEP_2);
        }
        else
        {
            nrf_gpio_pin_set(USER_PIN_BEEP_1);
            nrf_gpio_pin_clear(USER_PIN_BEEP_2);
        }
    }
    
    else
    {
        nrf_gpio_pin_set(USER_PIN_BEEP_1);
        nrf_gpio_pin_set(USER_PIN_BEEP_2);
    }

    beep_count++;
}

static void time_stamp_request_timeout_handler(void *p_context)
{
    is_need_request_time_stamp = true;
    //LOG_INFO("Time stamp request");
}

void timers_init()
{
    uint32_t err_code;

    err_code = app_timer_create(&m_temp_acq_timer_id, APP_TIMER_MODE_REPEATED, temp_acq_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_led_timer_id, APP_TIMER_MODE_REPEATED, led_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_beep_timer_id, APP_TIMER_MODE_REPEATED, beep_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_request_time_stamp_timer_id, APP_TIMER_MODE_REPEATED, time_stamp_request_timeout_handler);
    APP_ERROR_CHECK(err_code);

}

void timers_start()
{
    uint32_t err_code;
    err_code = app_timer_start(m_temp_acq_timer_id,TEMPERTURE_ACQUISITION_MEAS_INTERVAL,NULL);
    APP_ERROR_CHECK(err_code);
}

void timers_led_start()
{
    uint32_t err_code;
    err_code = app_timer_start(m_led_timer_id,LED_MEAS_INTERVAL,NULL);
    APP_ERROR_CHECK(err_code);
}

void timers_led_stop()
{
    app_timer_stop(m_led_timer_id);
    
}

void timers_stop()
{
    app_timer_stop(m_temp_acq_timer_id);
}

void timers_beep_start()
{
    //app_timer_start(m_beep_timer_id,LED_ME);
    app_timer_start(m_beep_timer_id, BEEP_MEAS_INTERVAL, NULL);
}

void timers_beep_stop()
{
    app_timer_stop(m_beep_timer_id);
    /*
    nrf_gpio_pin_set(22);
    nrf_gpio_pin_set(23);
    nrf_gpio_pin_set(22);
    nrf_gpio_pin_set(23);
    */
    //is_ble_connected = true;
}

void timers_time_stamp_request_start()
{
    app_timer_start(m_request_time_stamp_timer_id, TIME_STAMP_REQUEST_MEAS_INTERVAL, NULL);
}

void timers_time_stamp_request_stop()
{
    app_timer_stop(m_request_time_stamp_timer_id);
}

#include "nrf_gpio.h"
static nrf_drv_saadc_event_handler_t adc_handler(nrf_drv_saadc_evt_t const *pevent)
{

}
static void adc_init(void)
{
    nrf_gpio_cfg_output(6);
    ret_code_t err_code = nrf_drv_saadc_init(NULL, adc_handler);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN3);
    //config.gain = NRF_SAADC_GAIN1_6;
    config.gain = NRF_SAADC_GAIN1_3;
    //config.gain = NRF_SAADC_GAIN1;
    err_code = nrf_drv_saadc_channel_init(NRF_SAADC_INPUT_AIN3, &config);
    APP_ERROR_CHECK(err_code);

}

void user_app_init(void)
{
    uint32_t err_code;
    nrf_temp_init();
    uart_init();
    nrf_drv_gpiote_init();

    adc_init();

    ble_stack_init();
    
    gap_params_init();
    
    services_init();
    
    advertising_init();
    conn_params_init();
    
    //fs_init();
    user_storage2_init();
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    timers_init();

    is_device_registered = user_storage2_is_device_registered();
    if (is_device_registered)
    {
        timers_start();
    }
}