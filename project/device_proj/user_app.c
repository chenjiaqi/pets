/*
 * @File services.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-27 16:53:45 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-27 19:08:53
 */
#include "user_app.h"
#include "bsp.h"
#include "app_uart.h"
#include "nrf_drv_gpiote.h"

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

static void uart_event_handle(app_uart_evt_t * p_event)
{

}
static void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       //NULL,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
void user_app_init(void)
{
    //uart_init();
    nrf_drv_gpiote_init();

}