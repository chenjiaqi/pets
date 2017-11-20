/*
 * @File device_info.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-30 15:03:06 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-10-30 15:05:00
 */
#include "user_device_info.h"
#include "ble_gap.h"
#include "user_log.h"
#include "sdk_common.h"

uint32_t user_get_mac_address(uint8_t *p_addr)
{

    uint32_t err_code;
    ble_gap_addr_t gap_addr;
    uint8_t *p_gap_addr = NULL;

    if ( p_addr == NULL)
    {
        return NRF_ERROR_INVALID_ADDR;
    }


    memset(&gap_addr, 0, sizeof(gap_addr));

    gap_addr.addr_type = 0x07;

    err_code = sd_ble_gap_addr_get(&gap_addr);

    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    p_gap_addr = gap_addr.addr;

    for(int i = 0; i< BLE_GAP_ADDR_LEN; i++)
    {
//        LOG_INFO("%x", *(p_gap_addr + BLE_GAP_ADDR_LEN - i -1));
        *(p_addr + i) = *(p_gap_addr + BLE_GAP_ADDR_LEN - i -1);
    }

    return NRF_SUCCESS;
}

uint32_t user_get_mac_address_str(uint8_t *p_addr_str)
{
    if ( p_addr_str == NULL)
    {
        return NRF_ERROR_INVALID_ADDR;
    }
    uint32_t err_code;
    uint8_t addr_str[BLE_GAP_ADDR_LEN * 2 + 1];
    uint8_t addr[BLE_GAP_ADDR_LEN];

    err_code = user_get_mac_address(&addr);

    if (err_code != NRF_SUCCESS)
    {
        //LOG_ERROR("%x", LOG_UINT(err_code));
        return err_code;
    }
    
    //LOG_INFO("%2d", sizeof(addr_str));
    for(int i = 0; i <BLE_GAP_ADDR_LEN; i++)
    {
        sprintf(p_addr_str + i * 2, "%02X", addr[i]);
    }
    return NRF_SUCCESS;
}
