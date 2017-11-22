/*
 * @File lis3dh.h
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-17 13:50:51 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-17 14:02:32
 */

#ifndef LIS3DH_H
#define LIS3DH_H
#include <stdint.h>

#define SPI_LIS3DH_SS_PIN 13
#define SPI_LIS3DH_MISO_PIN 14
#define SPI_LIS3DH_MOSI_PIN 15
#define SPI_LIS3DH_SCK_PIN 17
#define SPI_LIS3DH_INSTANCE 0

void lis3dh_init();
void lis3dh_read_reg(uint8_t reg, uint8_t *data);
void lis3dh_write_reg(uint8_t reg, uint8_t data);
void lis3dh_test();

#endif

