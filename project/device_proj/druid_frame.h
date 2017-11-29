#ifndef DRUID_FRAME_H
#define DRUID_FRAME_H

#include <stdint.h>


#define MAX_SPLIT_SIZE 20
#define MAX_FRAME_SIZE 256

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
}druid_split_frame_t;

typedef struct _frame_head
{
    uint8_t seq;
    uint8_t cmd;
    uint16_t len;
}druid_frame_head_t;

typedef struct _trans_frame
{
    uint8_t seq;
    uint8_t cmd;
    uint16_t len;
    uint8_t *p_data;
}druid_frame_t;

void druid_set_construct_trans_frame(druid_frame_t frame);

void druid_set_split_frame(uint8_t const * p_data, uint16_t len);
uint8_t *get_split_frame(uint16_t *length);
void test_frame(uint8_t len);
uint8_t * druid_construct_frame(const uint8_t *p_split_frame, uint8_t len);
uint16_t druid_frame_get_frame_length();

#endif