#include "stdio.h"
#include "MyDHT11.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_uart.h"

#define DATA_PIN    11   
#define PIN_DATA_OUT   (nrf_gpio_cfg_output(DATA_PIN));  
#define PIN_DATA_IN    (nrf_gpio_cfg_input(DATA_PIN,NRF_GPIO_PIN_PULLUP)); 


#define PIN_DATA_SET    (nrf_gpio_pin_set(DATA_PIN)); 
#define PIN_DATA_CLEAR  (nrf_gpio_pin_clear(DATA_PIN));
    uint32_t array[5][8];
    


static uint8_t get_pin_value()
{
    return nrf_gpio_pin_read(DATA_PIN);
}
static uint32_t com(uint8_t byte)
{
    uint8_t n = 0,bit = 0;
    uint32_t count = 0;
    uint32_t count_zero = 0;
    for(int i = 0; i< 8; i ++)
    {
        count = 0;
        count_zero = 0;
        while(!get_pin_value());
        while(get_pin_value())
        {
            nrf_delay_us(60);
            array[byte][i] ++;
        }

    }
    return count;
}

uint8_t start_to_read(void)
{
    memset(array,0,40*sizeof(uint32_t));
    printf("start to read\r\n");
    PIN_DATA_OUT;
    PIN_DATA_CLEAR; //拉低
    nrf_delay_ms(40);
    PIN_DATA_SET; //拉高
    PIN_DATA_IN;
    nrf_delay_us(20);
    uint32_t a,b,c,d,e;
    

    uint8_t current_value = get_pin_value();
    if(current_value == 0x00)
    {
        while(!get_pin_value());
        while(get_pin_value());
        a = com(0);
        b = com(1);
        c = com(2);
        d = com(3);
        e = com(4);
        for(int i = 0; i< 5; i++)
        {
            for(int j = 0; j< 8; j++)
            {
                printf("%u ", array[i][j]);
            }
            printf("\r\n");
        }
    }
    else
    {
        printf("initial failed\r\n");
    }
}