#ifndef MY_DHT11_H
#define MY_DHT11_H


typedef struct _MyDHT11
{
    uint8_t humidityH;
    uint8_t humidityL;
    uint8_t temperatureH;
    uint8_t temperatureL;
}MyDHT11;

uint32_t start_to_read(MyDHT11 *p_value);
#endif