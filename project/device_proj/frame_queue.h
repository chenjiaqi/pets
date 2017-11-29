#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H
#include "stdbool.h"
#include "stdint.h"

#define MAX_FRAME_SIZE 256
bool frame_queue_is_empty();
void frame_queue_put(uint8_t *frame, uint16_t length);
uint16_t frame_queue_get(uint8_t *frame);
bool druid_frame_is_frame_valid(const uint8_t *frame, uint16_t length);
#endif