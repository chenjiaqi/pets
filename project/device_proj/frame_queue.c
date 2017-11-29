/*
 * @File undefined 
 * @Author: chenjiaqi@druid 
 * @Date: 2017-11-28 11:08:51 
 * @Last Modified by: chenjiaqi@druid
 * @Last Modified time: 2017-11-28 19:53:59
 */
#include "frame_queue.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "user_log.h"


#define MAX_QUEUE_SIZE 4
static uint8_t recv_queue[MAX_QUEUE_SIZE][MAX_FRAME_SIZE];
static uint16_t recv_queue_length[MAX_QUEUE_SIZE];
static uint8_t queue_font = 0;
static uint8_t queue_tail = 0;

void frame_queue_put(uint8_t *frame, uint16_t length)
{
    if((frame == NULL) || (length > MAX_FRAME_SIZE) || (length == 0))
    {
        return;
    }
    //LOG_INFO("Store a record %d", length);
    memcpy(&(recv_queue[queue_font][0]), frame, length);
    recv_queue_length[queue_font] = length;
    queue_font ++;
    if(queue_font >= MAX_QUEUE_SIZE)
    {
        queue_font = 0;
    }
}

uint16_t frame_queue_get(uint8_t *frame)
{
    uint16_t length = 0;
    if (frame_queue_is_empty() || (frame == NULL))
    {
        //LOG_INFO("empty");
        return length;
    }
    memcpy(frame,&(recv_queue[queue_tail][0]),recv_queue_length[queue_tail]);
    length = recv_queue_length[queue_tail];

    queue_tail ++;
    if(queue_tail >= MAX_QUEUE_SIZE)
    {
        queue_tail = 0;
    }
    //LOG_INFO("Get a record %d",length);
    return length;
}

bool frame_queue_is_empty()
{
    return (queue_font == queue_tail);
}


