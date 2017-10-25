/*
 * @File undefined 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-24 16:07:39 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-24 17:02:21
 */

#ifndef USER_UART_H
#define USER_UART_H

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */
#include <stdint.h>

void user_uart_test(void);
void user_uart_init(void);
uint32_t user_uart_close(void);
uint32_t user_uart_puts(const uint8_t *p_data, uint32_t length);

#endif // !USER_UART_H