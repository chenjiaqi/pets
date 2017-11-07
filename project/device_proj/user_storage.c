/*
 * @File user_storage.c 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-06 10:41:02 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-07 20:40:49
 */

#include "user_storage.h"
#include "user_log.h"
#include "fstorage.h"
static user_storage_t user_storage_info;


static bool fs_call_back_flag = true;


#define PAGE_NUM 8
#define PAGE_SIZE 1024

#define CUR_REL_POS (user_storage_info.p_current_write_addr - user_storage_info.p_start_addr)
#define CUR_READ_REL_POS (user_storage_info.p_current_read_addr - user_storage_info.p_start_addr)
#define IS_ADDRESS_IN_VALID_RANGE(p) ((p >= user_storage_info.p_start_addr)&&(p <= user_storage_info.p_end_addr)?true:false)

static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result);

FS_REGISTER_CFG(fs_config_t fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = (PAGE_NUM + 1),             // Number of physical flash pages required.
        .priority = 0xFE            // Priority for flash usage.
};


static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result)
{
    if (result != FS_SUCCESS)
    {
        //bsp_indication_set(BSP_INDICATE_FATAL_ERROR);
    }
    else
    {
        LOG_EVENT("fstorage command completed");
        fs_call_back_flag = true;
    }
}



#if 0 
static void store_temp_humity_to_flash()
{
    LOG_INFO("Current Store address is: %x", (unsigned int)(fs_config.p_start_addr + current_store_flash_point));
    //LOG_INFO("%x", *(fs_config.p_start_addr + current_store_flash_point));
    while(*(fs_config.p_start_addr + current_store_flash_point) != 0xFFFFFFFF)
    {
        current_store_flash_point++;
    }

    fs_call_back_flag = true;
    fs_store(&fs_config, fs_config.p_start_addr + current_store_flash_point, (uint32_t *)&user_flash_struct, sizeof(user_flash_struct)/sizeof(uint32_t), NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }
}
#endif

extern void power_manage();

void find_current_write_pos()
{
    LOG_INFO("%X", user_storage_info.p_current_write_addr);
    if (CUR_REL_POS % PAGE_SIZE == 0)
    {
        //LOG_INFO("reach page");
        while(((*(user_storage_info.p_current_write_addr)) & 0x03) == 0x00)
        {
            LOG_INFO("next page");
            user_storage_info.p_current_write_addr += PAGE_SIZE;

            if (user_storage_info.p_current_write_addr == user_storage_info.p_start_addr + PAGE_NUM * PAGE_SIZE)
            {
                user_storage_info.p_current_write_addr = user_storage_info.p_start_addr;
            }
        }
        user_storage_info.p_current_write_addr += 8; // skep to page head;
    }

    //while((*(user_storage_info.p_current_write_addr)) != 0xffffffff && (CUR_REL_POS % PAGE_SIZE))
    while((CUR_REL_POS % PAGE_SIZE) &&(*(user_storage_info.p_current_write_addr)) != 0xffffffff )
    {
        user_storage_info.p_current_write_addr += 2;
    }

    if (CUR_REL_POS % PAGE_SIZE == 0)
    {
        if (CUR_REL_POS == (PAGE_SIZE * PAGE_NUM))
        {
            /** <Reach the end of storage> */
            user_storage_info.p_current_write_addr = user_storage_info.p_start_addr;
        }
        fs_erase(&fs_config, user_storage_info.p_current_write_addr, 1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
        LOG_INFO("Erase SUCCESS\r\n");
        user_storage_info.p_current_write_addr += 8; //skip page head
    }

    LOG_INFO("Current pos is %X:%X", LOG_UINT(user_storage_info.p_current_write_addr), *(user_storage_info.p_current_write_addr));
}

void user_storage_init()
{
    fs_init();
    user_storage_info.p_start_addr = fs_config.p_start_addr + PAGE_SIZE;
    user_storage_info.p_end_addr = fs_config.p_end_addr;
    user_storage_info.p_current_write_addr = fs_config.p_start_addr + PAGE_SIZE;
    LOG_INFO("%X--->%X",LOG_UINT(user_storage_info.p_start_addr), LOG_UINT(user_storage_info.p_end_addr));
    //fs_store(&fs_config, fs_config.p_start_addr + 22, &a,1, NULL);
    //find_current_write_pos();
}

void user_store_to_flash(user_flash_structure_t *info)
{
    find_current_write_pos();
    fs_call_back_flag = false;
    static uint32_t page_flag;
    fs_store(&fs_config, user_storage_info.p_current_write_addr, (uint32_t *)info, sizeof(*info) / (sizeof(uint32_t)), NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }
    LOG_INFO("Write Over");

    if (CUR_REL_POS % PAGE_SIZE == 8)
    {
        LOG_INFO("Write flag");
        page_flag = 0xfffffffe;
        fs_store(&fs_config, user_storage_info.p_current_write_addr - 8, &page_flag, 1, NULL);
        fs_call_back_flag = false;
        while (!fs_call_back_flag)
        {
            power_manage();
        }
    }

    if (CUR_REL_POS % PAGE_SIZE == PAGE_SIZE -2) // last record in a page
    {
        page_flag = 0xfffffffd;
        fs_call_back_flag = false;
        fs_store(&fs_config, user_storage_info.p_current_write_addr - 1022, &page_flag, 1, NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
    }
}

static void find_read_pos_from_head(uint32_t *head)
{
    //LOG_INFO("head:%x", LOG_UINT(head));
    uint32_t i = 0, flag = 0, count = 0;
    if((head - user_storage_info.p_start_addr) % PAGE_SIZE)
    {
        LOG_ERROR("Invalid Param");
        return;
    }

    for(i = 0; i < 8; i++)
    {
        LOG_INFO("FLAG %d %X", i, head[i]);
        if (i == 0)
        {
            if((head[i] & 0xfffffffc) != 0x00 )
            {
                LOG_INFO("SUCCESS");
                break;
            }
        }
        else
        {
            if (head[i] != 0x00)
            {
                break;
            }
        }
    }

    flag = head[i];
    if(i == 0)
    {
        flag = flag & 0xfffffffc;
    }
    //LOG_INFO("FLAG is %X", head[i]);
    while((flag & 0x01) == 0)
    {
        flag = flag >> 1;
        count ++;
        if(count >= 32)
        {
            user_storage_info.p_current_read_addr = (void *)0;
            break;
        }
    }
    //LOG_INFO("CURRENT INDEX = %d, %d, %d", i, count, i*32 + count);
    //LOG_INFO("CURRENT ADDRESS is %X", head + (i * 32 + count) *4);
    user_storage_info.p_current_read_addr = head + (((i << 5) + count) << 2);
    //LOG_INFO("CURRENT ADDRESS is %X", user_storage_info.p_current_read_addr);
}

void user_storage_set_address(uint32_t p)
{
    user_storage_info.p_current_write_addr = user_storage_info.p_end_addr - 4;
    LOG_INFO("JUDGE POSITION %x", user_storage_info.p_current_write_addr);
}

/** <return current read pos> */
uint32_t *user_storage_get_current_read_position()
{
    uint32_t *p = user_storage_info.p_current_write_addr + (PAGE_SIZE - (CUR_REL_POS % PAGE_SIZE));
    /** <Not writted or not readed> */
    while (p != user_storage_info.p_end_addr && ((((*p) & 0x03) == 0x03) || (*(p + 7) == 0x00)))
    {
        p = p + PAGE_SIZE;
        if (p == user_storage_info.p_end_addr)
        {
            p = user_storage_info.p_start_addr;
        }
    }

    find_read_pos_from_head(p);
    return user_storage_info.p_current_read_addr;
}

uint32_t *user_storage_get_current_write_position()
{
    return user_storage_info.p_current_write_addr; 
}

uint32_t user_storage_get_a_record()
{
    if (user_storage_info.p_current_read_addr == 0x00) // first start up
    {
        uint32_t *p = user_storage_info.p_current_write_addr + (PAGE_SIZE - (CUR_REL_POS % PAGE_SIZE));
        LOG_INFO("%X", p);

        /** <Not writted or not readed> */
        while (p != user_storage_info.p_end_addr && ((((*p) & 0x03) == 0x03) || (*(p + 7) == 0x00)))
        {
            p = p + PAGE_SIZE;
            LOG_INFO("Find next page:%x", LOG_UINT(p));
            if (p == user_storage_info.p_end_addr)
            {
                p = user_storage_info.p_start_addr;
            }
        }

        find_read_pos_from_head(p);
        // get a record
        // change the flag
    }
    else
    {
        LOG_INFO("Read addr is:%X", user_storage_info.p_current_read_addr += 4);
        if (CUR_READ_REL_POS % PAGE_SIZE == 0)
        {
            if(user_storage_info.p_current_read_addr == user_storage_info.p_end_addr)
            {
                user_storage_info.p_current_read_addr = user_storage_info.p_start_addr;
            }
            user_storage_info.p_current_read_addr += 8; // skip head flag
        }
        // find next record
        // if reach a page end -> change a page
        // if reach the storage end -> change to the start addr
        // if all record has been read --> return NOTHING

    }
}

uint32_t * get_page_start_addr(uint32_t *p)
{
    return p - ((p-user_storage_info.p_start_addr) % PAGE_SIZE);
}


void user_storage_set_has_been_transformed(uint32_t * from, uint32_t *to)
{
    uint32_t *from_page_start = NULL;
    uint32_t *to_page_start = NULL;
    if(!(IS_ADDRESS_IN_VALID_RANGE(from)&&IS_ADDRESS_IN_VALID_RANGE(to)))
    {
        LOG_ERROR("Invalid param");
    }
    LOG_INFO("%X--->%X", from, to);
    LOG_INFO("%d", to - from);
    from_page_start = get_page_start_addr(from);
    to_page_start = get_page_start_addr(to);


    if (get_page_start_addr(from) == get_page_start_addr(to))
    {
        LOG_INFO("IN A PAGE");
        //LOG_INFO("%d, %d",(from - from_page_start)>>6, ((from - from_page_start) >> 2) % 16);
        //LOG_INFO("%d, %d",(to - to_page_start)>>6, ((to - to_page_start) >> 2) % 16);

        uint8_t start_word_pos, end_word_pos, start_bits_pos, end_bits_pos;

        start_word_pos = (from - from_page_start) >> 6;
        start_bits_pos =  ((from - from_page_start) >> 2) % 16;

        end_word_pos = (to - to_page_start) >> 6;
        end_bits_pos = ((to - to_page_start) >> 2) % 16;

        LOG_INFO("%d, %d",start_word_pos, start_bits_pos);
        LOG_INFO("%d, %d",end_word_pos, end_bits_pos);

        for(int i = start_word_pos; i < end_word_pos; i++)
        {

        }
    }
    else
    {
    }
}

void ble_storage_set_register()
{
    //uint32_t *p_register_info = user_storage_info.p_start_addr;
    uint32_t *p_register_info = fs_config.p_start_addr;
    LOG_INFO("%x", *p_register_info);
}

