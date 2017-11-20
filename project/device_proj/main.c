/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "user_log.h"
#include "user_uart.h"
#include "nrf_log.h"
#include "user_ble.h"
#include "user_ble_device_manages.h"
#include "user_ble_srv_common.h"
#include "user_app.h"
#include "MyDHT11.h"
#include "user_device_info.h"
#include "user_process.h"
#include "nrf_delay.h"
#include "global.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0 /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE GATT_MTU_SIZE_DEFAULT /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2 /**< Reply when unsupported features are requested. */

#define APP_TIMER_PRESCALER 0     /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 4 /**< Size of timer operation queues. */


#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
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

bool is_need_read = false;
bool is_need_write_flash = false;
bool is_need_read_flash = false;

void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
    case BSP_EVENT_SLEEP:
        LOG_PROC("EVENT", "SLEEP ENTER");
        sleep_mode_enter();
        break;

    case BSP_EVENT_DISCONNECT:
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
        }
        break;

    case BSP_EVENT_WHITELIST_OFF:
        if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
        {
            err_code = ble_advertising_restart_without_whitelist();
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
        }
        break;
    case BSP_EVENT_KEY_1:
        LOG_EVENT("KEY1 PRESSED");
        //LOG_ERROR("ERROR");
        //sleep_mode_enter();

        //start_to_read();
        is_need_read = true;
        break;
    case BSP_EVENT_KEY_2:
        LOG_EVENT("KEY2 PRESSED:is_NEED_READ_FLSAH");
        is_need_read_flash = true;
        break;
    case BSP_EVENT_KEY_3:
        LOG_EVENT("KEY3 PRESSED");
        is_need_write_flash = true;
        break;
    default:
        break;
    }
}

static void buttons_leds_init(bool *p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}

/**@brief Function for placing the application in low power state while waiting for events.
 */
void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

//extern user_ble_device_manage_t m_device_manager;
/**@brief Application main function.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    //buttons_leds_init(&erase_bonds);

    nrf_gpio_cfg_output(LED_4);
    nrf_gpio_pin_set(LED_4);
    user_app_init();
    //nrf_gpio_cfg_default(5);
    //nrf_gpio_cfg_default(7);

    //nrf_gpio_pin_clear(LED_4);

    
    //nrf_gpio_pin_clear(LED_4);

    nrf_gpio_cfg_output(29);
    nrf_gpio_pin_clear(29);
    //nrf_gpio_pin_clear(29);
    nrf_gpio_pin_set(29);
    //printf("start\r\n");
    nrf_gpio_cfg_output(22);
    nrf_gpio_cfg_output(23);
    
    nrf_gpio_cfg(
        22,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1,
        NRF_GPIO_PIN_NOSENSE);

        nrf_gpio_cfg(
            23,
            NRF_GPIO_PIN_DIR_OUTPUT,
            NRF_GPIO_PIN_INPUT_DISCONNECT,
            NRF_GPIO_PIN_NOPULL,
            NRF_GPIO_PIN_H0H1,
            NRF_GPIO_PIN_NOSENSE);

        nrf_gpio_pin_clear(22);
        nrf_gpio_pin_clear(23);

    
    //LOG_INFO("PETS_PROJ START");
    // Enter main loop.
    uint8_t addr_str[(BLE_GAP_ADDR_LEN << 1) + 1];
    user_get_mac_address_str(addr_str);
    //LOG_INFO("MAC ADDRESS is %s", addr_str);
    
   //nrf_gpio_pin_clear(29); 

    for (;;)
    {
        
#if 0
        uint8_t m=180;
        uint32_t count = 0;
        while(1)
        {
            nrf_gpio_pin_clear(22);
            nrf_gpio_pin_set(23);
            //nrf_delay_ms(m);
            nrf_delay_us(m);
            nrf_gpio_pin_clear(23);
            nrf_gpio_pin_set(22);
            //nrf_delay_ms(m);
            nrf_delay_us(m);

            if(count % 300 == 0)
            {
                nrf_gpio_pin_clear(23);
                nrf_gpio_pin_clear(22);
                nrf_delay_ms(500);
                //nrf_gpio_pin_clear(29); 
            }

            count ++;

            /*
            nrf_gpio_pin_clear(23);
            nrf_gpio_pin_clear(22);
            nrf_delay_ms(1000)
            */
        }
#endif

        user_process();
        power_manage();
    }
}

/**
 * @}
 */
