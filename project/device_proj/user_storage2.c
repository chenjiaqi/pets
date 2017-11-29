#include "user_storage2.h"
#include "fstorage.h"
#include "user_log.h"

#define PAGE_NUM 8
#define PAGE_SIZE 1024

volatile static bool fs_call_back_flag = false;

static user_storage2_t user_storage_info;




#define CURRENT_REL_WRITE_POS (user_storage_info.p_current_write_addr - user_storage_info.p_start_addr)
#define IS_CURRENT_WRITE_REACH_END (user_storage_info.p_current_write_addr == user_storage_info.p_end_addr)


static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result)
{
    if (result != FS_SUCCESS)
    {
        //bsp_indication_set(BSP_INDICATE_FATAL_ERROR);
    }
    else
    {
//        LOG_EVENT("fstorage command completed");
        fs_call_back_flag = true;
    }
}

FS_REGISTER_CFG(fs_config_t data_fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = (PAGE_NUM * 10),             // Number of physical flash pages required.
        .priority = 0xFE            // Priority for flash usage.
};

FS_REGISTER_CFG(fs_config_t info_fs_config) =
{
        .callback = fs_evt_handler, // Function for event callbacks.
        .num_pages = 1,             // Number of physical flash pages required.
        .priority = 0xFE            // Priority for flash usage.
};


void user_storage2_init()
{
    fs_init();
    //LOG_INFO("DATA:%X-->%X,INFO:%X-->%X",LOG_UINT(data_fs_config.p_start_addr), LOG_UINT(data_fs_config.p_end_addr),LOG_UINT(info_fs_config.p_start_addr), LOG_UINT(info_fs_config.p_end_addr));
    user_storage_info.p_start_addr = data_fs_config.p_start_addr;
    user_storage_info.p_end_addr = data_fs_config.p_end_addr;
    user_storage_info.p_current_write_addr = data_fs_config.p_start_addr;
    user_storage_info.p_current_read_addr = data_fs_config.p_start_addr;
    //LOG_INFO("INFO:%X-->%X",LOG_UINT(info_fs_config.p_start_addr), LOG_UINT(info_fs_config.p_end_addr));
    find_current_write_pos();
}

extern void power_manage();
void user_storage2_store_device_information(uint32_t *info, uint16_t len)
{
    fs_call_back_flag = false;
    fs_store(&info_fs_config, info_fs_config.p_start_addr + 4, info, len, NULL);
    while(!fs_call_back_flag)
    {
        power_manage();
    }
}

bool user_storage2_is_device_registered()
{
    return ((*info_fs_config.p_start_addr) == 0xffffffff)?false:true;
}

bool user_storage2_register_device()
{
    LOG_PROC("REGISTER","ENTER");
    static uint32_t register_flag = 0x00;
    fs_call_back_flag = false;

    fs_store(&info_fs_config, info_fs_config.p_start_addr,&register_flag, 1, NULL);

    while(fs_call_back_flag == false)
    {
        //LOG_INFO("..");
        power_manage();
    }
    LOG_PROC("REGISTER","OVER");
    return user_storage2_is_device_registered();
}

bool user_storage2_unregister_device()
{
    LOG_PROC("UNREGISTER","ENTER");
    fs_call_back_flag = false;
    fs_erase(&info_fs_config, info_fs_config.p_start_addr, 1,NULL);
    while(fs_call_back_flag == false)
    {
        power_manage();
    }
    fs_call_back_flag = false;

    fs_erase(&data_fs_config, data_fs_config.p_start_addr, 1,NULL);
    while(fs_call_back_flag == false)
    {
        power_manage();
    }

    user_storage_info.p_start_addr = data_fs_config.p_start_addr;
    user_storage_info.p_end_addr = data_fs_config.p_end_addr;
    user_storage_info.p_current_write_addr = data_fs_config.p_start_addr;
    user_storage_info.p_current_read_addr = data_fs_config.p_start_addr;
    LOG_PROC("UNREGISTER","OVER");
 

    return true;
}

static uint32_t *find_next_write_page()
{
    uint32_t *next_page;
    next_page = (user_storage_info.p_current_write_addr - (CURRENT_REL_WRITE_POS % PAGE_SIZE)) + PAGE_SIZE;
    if (next_page == user_storage_info.p_end_addr)
    {
        next_page = user_storage_info.p_start_addr;
    }
    return next_page;
}


void find_current_write_pos()
{
    uint32_t * p_write = user_storage_info.p_current_write_addr;
    if ((!IS_CURRENT_WRITE_REACH_END) && CURRENT_REL_WRITE_POS % 1024 == 0)
    {
        while(*(p_write) != 0xffffffff && *(p_write + PAGE_SIZE - 2) != 0xffffffff)
        {
            p_write = p_write + PAGE_SIZE;
            if (p_write == user_storage_info.p_end_addr)
            {
                p_write = user_storage_info.p_start_addr;
                break;
            }
        }
    }
    //LOG_INFO("ADDRESS:%X", p_write);


    while(*p_write != 0xffffffff)
    {
        p_write = p_write + 2;

        if(p_write == user_storage_info.p_end_addr)
        {
            p_write = user_storage_info.p_start_addr;
            break;
        }
    }
    user_storage_info.p_current_write_addr = p_write;

    // erase the next page
    if(CURRENT_REL_WRITE_POS % PAGE_SIZE == 0)
    {
        // if read_pos is in this zone
        if ((user_storage_info.p_current_read_addr >= user_storage_info.p_current_write_addr) && (user_storage_info.p_current_read_addr <= user_storage_info.p_current_write_addr + PAGE_SIZE - 1))
        {
            uint32_t *next_page = find_next_write_page();
            if (*next_page == 0xffffffff) // not written
            {
                // not written, do nothing
            }
            else // next page is written
            {
                // put next page 
                user_storage_info.p_current_read_addr = next_page;
            }
        }

        fs_call_back_flag = false;
        fs_erase(&data_fs_config, user_storage_info.p_current_write_addr,1,NULL);
        while(!fs_call_back_flag)
        {
            power_manage();
        }
    }
}

void user_storage2_store_a_record(user_flash_structure_t * data)
{
    find_current_write_pos();
    LOG_INFO("Current pos is:%X--[%X]", user_storage_info.p_current_write_addr, *(user_storage_info.p_current_write_addr));
    fs_call_back_flag = false;
    fs_store(&data_fs_config, user_storage_info.p_current_write_addr, data, 2, NULL);
    while(!fs_call_back_flag)
    {
        power_manage();
    }
}

static void get_current_read_pos_from_flash()
{
    if(*(info_fs_config.p_end_addr - 1) != 0xffffffff)
    {
        user_storage_info.p_current_read_addr = *(info_fs_config.p_end_addr - 1);
    }
    else
    {
        user_storage_info.p_current_read_addr = user_storage_info.p_start_addr;
    }
}

static void store_current_read_pos_to_flash()
{
    
    static uint32_t device_info[32];
    // get device info
    memcpy(device_info, info_fs_config.p_start_addr, 32);

    // erase page
    fs_call_back_flag = false;
    fs_erase(&info_fs_config, info_fs_config.p_start_addr, 1, NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }

    // store device info to flash
    fs_call_back_flag = false;
    fs_store(&info_fs_config, info_fs_config.p_start_addr, device_info, 32, NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }

    // store current read pos into flash
    fs_call_back_flag = false;
    fs_store(&info_fs_config, info_fs_config.p_end_addr - 1, &(user_storage_info.p_current_read_addr), 1, NULL);
    while (!fs_call_back_flag)
    {
        power_manage();
    }
}

static bool is_all_data_read_out()
{
    if ((user_storage_info.p_current_write_addr == user_storage_info.p_current_read_addr) ||
        (user_storage_info.p_current_read_addr == (user_storage_info.p_current_write_addr - 2)))
    {
        return true;
    }
    return false;
}

uint32_t * user_storage2_get_a_record()
{
    uint32_t *p = user_storage_info.p_current_read_addr;
    if(is_all_data_read_out())
    {
        return NULL;
    }
    user_storage_info.p_current_read_addr += 2;

    // reach the end of storage
    if(user_storage_info.p_current_read_addr == user_storage_info.p_end_addr)
    {
        user_storage_info.p_current_read_addr = user_storage_info.p_start_addr;
    }
    return p;
}

uint32_t user_storage2_get_record_count()
{
    //LOG_INFO("Current read address is 0x%X-%X:%d", LOG_UINT(user_storage_info.p_current_read_addr),
    //LOG_UINT(user_storage_info.p_current_write_addr), (user_storage_info.p_current_write_addr - user_storage_info.p_current_read_addr));
    if(user_storage_info.p_current_read_addr <= user_storage_info.p_current_write_addr)
    {
        //LOG_INFO("%d",((user_storage_info.p_current_write_addr - user_storage_info.p_current_read_addr) / 2));
        return ((user_storage_info.p_current_write_addr - user_storage_info.p_current_read_addr) / 2);
    }
    return (user_storage_info.p_current_write_addr - user_storage_info.p_start_addr + user_storage_info.p_end_addr - user_storage_info.p_current_read_addr) /2;
}

void user_storage2_test_set_read_addr()
{
    user_storage_info.p_current_read_addr = (void *)0x7eff0;
    user_storage_info.p_current_write_addr = (void *)0x7eff0;
}

