#ifndef USER_STORAGE2_H
#define USER_STORAGE2_H

#include <stdint.h>
#include <stdbool.h>
#include "user_process.h"

typedef struct user_storage2
{
    uint32_t *p_current_write_addr; // current write pos
    uint32_t *p_start_addr;         // flash start addr
    uint32_t *p_end_addr;           // flash end addr
    uint32_t *p_current_read_addr;  // current read addr
}user_storage2_t;

void user_storage2_init();
bool user_storage2_is_device_registered();
bool user_storage2_register_device();
bool user_storage2_unregister_device();
void user_storage2_store_a_record(user_flash_structure_t * data);
uint32_t * user_storage2_get_a_record();

uint32_t user_storage2_get_record_count();
void user_storage2_test_set_read_addr();

#endif 
