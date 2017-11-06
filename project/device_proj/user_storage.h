/*
 * @File user_storage.h 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-06 10:41:26 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-06 17:57:22
 */

#ifndef USER_STORAGE_H
#define USER_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "user_process.h"

typedef struct user_storage
{
    uint32_t *p_current_write_addr; // current write pos
    uint32_t *p_start_addr;         // flash start addr
    uint32_t *p_end_addr;           // flash end addr
    uint32_t *p_current_read_addr;  // current read addr
}user_storage_t;


void user_storage_init();
void find_current_write_pos();
void user_store_to_flash(user_flash_structure_t * info);
void user_storage_set_address(uint32_t p);
uint32_t user_storage_get_a_record();


#endif 
