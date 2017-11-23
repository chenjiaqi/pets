/*
 * @File lis3dh.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-17 13:50:30 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-17 14:28:00
 */
#include "lis3dh.h"
#include "nrf.h"
#include "app_error.h"
#include "user_log.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_LIS3DH_INSTANCE);  /**< SPI instance. */
static uint8_t       m_tx_buf[2];           /**< TX buffer. */
static uint8_t       m_rx_buf[2];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */
volatile static bool is_spi_trans_success = false;

/**
 * @brief SPI user event handler.
 * @param event
 */

static void spi_event_handler(nrf_drv_spi_evt_t const *p_event,
                       void *p_context)
{
    is_spi_trans_success = true;
    //LOG_INFO("xx");
}

static void lis3dh_spi_init()
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_LIS3DH_SS_PIN;
    spi_config.miso_pin = SPI_LIS3DH_MISO_PIN;
    spi_config.mosi_pin = SPI_LIS3DH_MOSI_PIN;
    spi_config.sck_pin  = SPI_LIS3DH_SCK_PIN;
    (nrf_drv_spi_init(&spi, &spi_config, spi_event_handler));
}
void lis3dh_init()
{
    lis3dh_spi_init();
    //lis3dh_write_reg(0x24,0x80);
    nrf_delay_ms(10);
    //lis3dh_write_reg(0x20,0x3f);
    lis3dh_write_reg(0x20,0x1f);
    //lis3dh_write_reg(0x21,0x09);
    lis3dh_write_reg(0x21,0x01);
    //lis3dh_write_reg(0x22,0x40);
    lis3dh_write_reg(0x22,0x40 | 0x80);
    lis3dh_write_reg(0x23,0x00);

    //lis3dh_write_reg(0x24,0x08);
    lis3dh_write_reg(0x24,0x00);
    lis3dh_write_reg(0x32,0x25); // 0x35 is default

#if 0
    lis3dh_write_reg(0x38, 0x20);
    lis3dh_write_reg(0x3a, 0x10);
#endif

    //lis3dh_write_reg(0x31,0x7f);
    lis3dh_write_reg(0x33,0x00);
    lis3dh_read_reg(0x26, NULL);
    //lis3dh_write_reg(0x30, 0x2a);
    lis3dh_write_reg(0x30, 0xaa);

    /** <Stop SPI save energy> */
    nrf_drv_spi_uninit(&spi);
    nrf_gpio_cfg_default(SPI_LIS3DH_SS_PIN);
    nrf_gpio_cfg_default(SPI_LIS3DH_MISO_PIN);
    nrf_gpio_cfg_default(SPI_LIS3DH_MISO_PIN);
    nrf_gpio_cfg_default(SPI_LIS3DH_SCK_PIN);

}

void lis3dh_read_reg(uint8_t reg, uint8_t *data)
{
    m_tx_buf[0] = reg | 0x80;
    m_tx_buf[1] = 0x00;
    is_spi_trans_success = false;
    nrf_drv_spi_transfer(&spi, m_tx_buf,m_length, m_rx_buf, m_length);
    while(!is_spi_trans_success);
    if (data)
    {
        *data = m_rx_buf[1];
    }
}

void lis3dh_write_reg(uint8_t address, uint8_t data)
{
    m_tx_buf[0] = address;
    m_tx_buf[1] = data;
    is_spi_trans_success = false;
    nrf_drv_spi_transfer(&spi, m_tx_buf,m_length, m_rx_buf, m_length);
    while(!is_spi_trans_success);
}


void lis3dh_test()
{
    uint8_t reg, data;
    reg = 0x0f | 0x80;
    //reg = 0x8f;
    //lis3dh_read_reg(reg,&data);
//    lis3dh_write_reg(0x24, 0x80);

//    LIS3DH_GetWHO_AM_I(&data);
    uint8_t x_l, x_h, y_h, y_l, z_h, z_l;
    int16_t x,y,z;
    lis3dh_read_reg(0x28,&x_l);
    lis3dh_read_reg(0x29,&x_h);
    lis3dh_read_reg(0x2a,&y_l);
    lis3dh_read_reg(0x2b,&y_h);
    lis3dh_read_reg(0x2c,&z_l);
    lis3dh_read_reg(0x2d,&z_h);
    x = ((x_h << 8) | x_l);
    y = ((y_h << 8) | y_l);
    z = ((z_h << 8) | z_l);
    //LOG_INFO("%d    %d   %d", x, y, z);


/*
    AxesRaw_t axesraw;
    LIS3DH_GetAccAxesRaw(&axesraw);
    LOG_INFO("%d    %d   %d", axesraw.AXIS_X, axesraw.AXIS_Y,axesraw.AXIS_Z);
*/
}
