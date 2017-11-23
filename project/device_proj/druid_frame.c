#include "druid_frame.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
//#include "messages.pb.h"
#include "messages.pb.h"
#include "user_log.h"

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