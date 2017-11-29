#include "druid_frame.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
//#include "messages.pb.h"
#include "messages.pb.h"
#include "user_log.h"
#include <stdio.h>

#define VERSION 0x10
//#define MAX_SPLIT_SIZE 20
//#define MAX_FRAME_SIZE 256

uint8_t const *p_data_start = NULL;
uint8_t const *p_current = NULL;
uint32_t left_len = 0;
static uint16_t current_frame_length = 0;

static uint8_t trans_frame_buf[MAX_FRAME_SIZE];

static uint8_t recv_frame_buf[MAX_FRAME_SIZE];
static uint8_t params_buf[MAX_FRAME_SIZE];
static uint8_t split_recv_frame_buf;


#if 0
enum split_group_type
{
    FRAME_TYPE_MIDDLE = 0x00,
    FRAME_TYPE_START= 0x01,
    FRAME_TYPE_LAST = 0x02,
    FRAME_TYPE_TOTAL = 0x03
};

typedef struct _split_frame
{
    uint8_t type;
    uint8_t split_frame[MAX_SPLIT_SIZE];
}split_frame;
#endif

static druid_split_frame_t frame;



void druid_set_split_frame(uint8_t const * p_data, uint16_t len)
{
    p_data_start = p_data;
    p_current = p_data;
    left_len = len;
}

uint8_t *get_split_frame(uint16_t *length)
{

    if(left_len <= 0 || p_current == NULL)
    {
        return NULL;
    }

    if (left_len <= MAX_SPLIT_SIZE - 1)
    {
        memcpy(frame.split_frame, p_current, left_len);
        if (p_current == p_data_start) // total
        {
            *length = left_len + 1;
            frame.type = FRAME_TYPE_TOTAL;
            left_len = 0;
        }
        else // last
        {
            *length = left_len + 1;
            frame.type = FRAME_TYPE_LAST;
            left_len = 0;
        }
        p_current = NULL;
    }
    else
    {
        memcpy(frame.split_frame, p_current, MAX_SPLIT_SIZE - 1);
        if (p_current == p_data_start) //start
        {
            *length = MAX_SPLIT_SIZE;
            frame.type = FRAME_TYPE_START;
            left_len = left_len - (MAX_SPLIT_SIZE-1);

        }
        else //middle
        {
            *length = MAX_SPLIT_SIZE;
            frame.type = FRAME_TYPE_MIDDLE;
            left_len = left_len - (MAX_SPLIT_SIZE-1);
        }
        p_current = p_current + (MAX_SPLIT_SIZE- 1);
    }
    return (uint8_t *)(&frame);
}

void test_frame(uint8_t len)
{
#if 0
    printf("TEST----LEN is %d\r\n", len);
    uint8_t mydata[100] = {0};
    for(int i = 0; i < len; i++)
    {
        mydata[i] = i;
    }
    druid_set_split_frame(mydata, len);
    uint8_t * p ;
    uint16_t length;
    uint8_t count = 0;
    do{
        p = get_split_frame(&length);
        if(p)
        {
                printf("\r\n[%d]:length is:%d\r\n", count, length);
                for(int j = 0; j < length; j++)
                {
                        printf("%d ", p[j]);
                }
                count ++;
        }

    }while(p);
    printf("\r\nEND TEST----LEN is %d\r\n", len);
#endif
}

void druid_set_construct_trans_frame(druid_frame_t frame)
{
    uint16_t len_trans = 0;
    trans_frame_buf[0] = frame.seq;
    trans_frame_buf[1] = frame.cmd;
    uint16_t *p_len = (uint16_t *)(&(trans_frame_buf[2]));
    *p_len = frame.len;
    memcpy(trans_frame_buf + 4, frame.p_data, frame.len);
    trans_frame_buf[4 +frame.len] = 0xff;
    trans_frame_buf[5 +frame.len] = 0xff;

    len_trans = frame.len + 6;

    druid_set_split_frame(trans_frame_buf,len_trans);
}

uint16_t calculate_check_sum(uint8_t *p_data, uint16_t len)
{
    return 0xffff;
}


uint8_t *druid_construct_frame(const uint8_t *p_split_frame, uint8_t length)
{
    bool is_complete = false;

    static uint8_t *p_current = NULL;
    static bool is_in_construct_mode = false;

    uint8_t split_type = p_split_frame[0] & 0x0f;
    switch(split_type)
    {
        case FRAME_TYPE_START:
        {
            p_current = recv_frame_buf;
            memcpy(p_current, p_split_frame + 1, length - 1);// remove split information
            p_current = p_current + length  - 1;
            is_in_construct_mode = true;
            current_frame_length = length - 1;
            break;
        }

        case FRAME_TYPE_MIDDLE:
        {
            if(is_in_construct_mode)
            {
                memcpy(p_current, p_split_frame + 1, length - 1);// remove split information
                p_current = p_current +length - 1;
                current_frame_length += length-1;

                if (p_current >= recv_frame_buf +MAX_FRAME_SIZE)
                {
                    is_in_construct_mode = false;
                }
            }
            break;
        }

        case FRAME_TYPE_LAST:
        {
            if(is_in_construct_mode)
            {
                memcpy(p_current, p_split_frame + 1, length - 1);// remove split information
                is_in_construct_mode = false;
                is_complete = true;
                current_frame_length += length - 1;
            }
            else
            {
                is_in_construct_mode = false;
                is_complete = false;
            }
            break;
        }

        case FRAME_TYPE_TOTAL:
        {
            p_current = recv_frame_buf;
            memcpy(p_current, p_split_frame + 1, length - 1);// remove split information
            is_in_construct_mode = false;
            current_frame_length = length - 1;
            is_complete = true;
            break;
        }

        default:
        {
            break;
        }
    }

    if (is_complete)
    {
        return recv_frame_buf;
    }
    return NULL;
}

uint16_t druid_frame_get_frame_length()
{
    return current_frame_length;
}

bool druid_frame_is_frame_valid(const uint8_t *frame, uint16_t length)
{
    bool ret = false;
    druid_frame_head_t *p_frame_head = (druid_frame_head_t *)frame;
    uint16_t *p_checksum = (uint16_t *)(frame + length - 2);
    if((p_frame_head->len == length - 6) && ((*p_checksum) == 0xffff))
    {
        ret = true;
    }
    return ret;
}

druid_frame_t *druid_frame_get_frame_info(uint8_t *frame_buf, uint16_t length)
{
    static druid_frame_t frame;
    uint16_t *p_checksum = (uint16_t *)(frame_buf + length - 2);
    frame = *((druid_frame_t *)(frame_buf));
    if((frame.len == length - 6) && ((*p_checksum) == 0xffff))
    {
        memcpy(params_buf, frame_buf + 4, frame.len);
        frame.p_data = params_buf;
        return &frame;
    }
    return NULL;
}




#if 0
static uint8_t buffer[128];

void druid_frame_test()
{
    LOG_INFO("druid");
    RequestMessage req_msg = RequestMessage_init_zero;
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    req_msg.frametype = FrameType_E_FRAME_REQUEST;
    req_msg.has_cmd = true;
    req_msg.cmd = Command_E_CMD_AUTH;
    pb_encode(&ostream, RequestMessage_fields, &req_msg);
    LOG_INFO("%d",ostream.bytes_written);
}
#endif