#include "stdio.h"
#include "MyDHT11.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_uart.h"
#include "nrf_error.h"
#include "user_log.h"

#define DATA_PIN    11   
#define PIN_DATA_OUT   (nrf_gpio_cfg_output(DATA_PIN));  
#define PIN_DATA_IN    (nrf_gpio_cfg_input(DATA_PIN,NRF_GPIO_PIN_PULLUP)); 


#define PIN_DATA_SET    (nrf_gpio_pin_set(DATA_PIN)); 
#define PIN_DATA_CLEAR  (nrf_gpio_pin_clear(DATA_PIN));



static uint8_t get_pin_value()
{
    return nrf_gpio_pin_read(DATA_PIN);
}
static uint8_t com(/*uint8_t byte*/)
{
    uint8_t n = 0,bit = 0;
    uint8_t count = 1;

    for(int i = 0; i< 8; i ++)
    {
        bit = 0;
        while(!get_pin_value()&&count++)
        {
            nrf_delay_us(1);
       };

        nrf_delay_us(65);

        if(get_pin_value())
        {
            bit = 1;
        }
        n <<= 1;
        n = n | bit;

        count = 200;
        while(get_pin_value() && count++)
        {
            nrf_delay_us(1);
        };
    }
    return n;
}

uint32_t start_to_read(MyDHT11 *p_value)
{
    uint32_t err_code = NRF_SUCCESS;
    uint8_t count;
    if( p_value == NULL)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    /** <Initial DHT11 >**/
    PIN_DATA_OUT;
    PIN_DATA_CLEAR;                 // Pull down the bus
    nrf_delay_ms(20);
    PIN_DATA_SET;                   // Pull up the data bus
    PIN_DATA_IN;
    nrf_delay_us(40);               // Wait for DTH11 pull down the buss
    uint8_t crc = 0;

    uint8_t current_value = get_pin_value();  

    if(current_value == 0x00)       // Check if the DTH11 initialization success or not
    {
        /* <Initialization Success>*/
        count = 150; 
        while(!get_pin_value()&&count++)
        {
            nrf_delay_us(1);
        };

        count = 150;
        while(get_pin_value()&& count++)
        {
            nrf_delay_us(1);
        };
        p_value->humidityH= com();
        crc += p_value->humidityH;

        p_value->humidityL = com();
        crc += p_value->humidityL;

        p_value->temperatureH = com();
        crc += p_value->temperatureH;

        p_value->temperatureL = com();
        crc += p_value->temperatureL;

        /** <Check if crc is correct>**/
        
        if(crc == com())
        {
            return NRF_SUCCESS;
            
        }
        else
        {
            LOG_ERROR("CRC_ERROR");
            err_code = NRF_ERROR_INVALID_DATA;
        }
    }
    else
    {
        LOG_ERROR("DHT11 INITIAL ERROR");
        return NRF_ERROR_INVALID_STATE;
    }
    return err_code;
}