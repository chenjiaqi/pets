/*
 * @File user_process.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-10-30 17:11:44 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-02 11:06:40
 */

#ifndef USER_PROCESS_H
#define USER_PROCESS_H

#include "user_process.h"
#include <stdint.h>
#include <stdbool.h>

void user_process(void);

typedef struct user_process_flag
{
    bool is_need_collect_temperture;
    bool is_need_update_time_stap;
}user_process_flag;

typedef struct user_flash_structure
{
    uint32_t time_stamp;
    uint8_t  temperture0;
    uint8_t  humidity0;
    uint8_t  temperture1;
    uint8_t  humidity1;
} user_flash_structure_t;


#endif

