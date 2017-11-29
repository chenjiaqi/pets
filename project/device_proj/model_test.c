#include "model_test.h"
#include <stdio.h>
#include "druid_frame.h"
#include "aes.h"
#include "messages.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

void test_druid_frame()
{
    test_frame(20);
    uint8_t data[100] = {0x01,0x02, 0x03, 0x04, 0x05};
    for(int i = 0; i< 100; i++)
    {
        data[i] = i;
    }
    druid_frame_t frame;
    frame.seq = 2;
    frame.cmd = 1;
    frame.len = 50;
    frame.p_data = data;
    druid_set_construct_trans_frame(frame);

    uint8_t *p;
    uint16_t length;
    printf("\r\n");
    do{
        p = get_split_frame(&length); 
        if(p)
        {
            for(int i = 0; i <length; i++)
            {
                printf("%02x ", *(p + i));
            }
            printf("\r\n");

            uint8_t *p_frame = druid_construct_frame(p, length);
            if(p_frame)
            {
                uint16_t *length = (uint16_t *)(p_frame +2);
                printf("\r\n*****%02x,%02x,%02x,%02x****%d\r\n",p_frame[0], p_frame[1],p_frame[2], p_frame[3], *length);
            }
        }
    }while(p);
    printf("\r\n");
}

void test_aes()
{
    uint8_t data[128];
    uint8_t encrypt_data[128];
    uint8_t decode_data[128];
    uint8_t key[16];
    for(int i = 0; i < 128; i++)
    {
        data[i] = i;
        encrypt_data[i] = 0;
        decode_data[i] = 0;
        if(i < 16)
        {
            key[i] = 22 - i;
        }
    }
    AES128_ECB_encrypt(data, key, encrypt_data);    
    AES128_ECB_decrypt(encrypt_data,key,decode_data);
    printf("\r\n");
    for(int i = 0; i <16; i++)
    {
        printf("%02x ",decode_data[i]);
    }
    printf("\r\n");
}

void test_proto()
{
    uint8_t buffer[128];
    /*
    RespGetDeviceParams dev_info = RespGetDeviceParams_init_zero;
    dev_info.device_id.size = 10;
    dev_info.has_led_status = true;
    dev_info.has_beep_status = true;
    dev_info.beep_status = true;
    dev_info.led_status = true;
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, RespGetDeviceParams_fields, &dev_info);
    printf("%d\r\n", ostream.bytes_written);
    */
    uint16_t len ;
    len = user_cmd_create_cmd_package(Command_E_CMD_GET_STORE_DATA,buffer);
    
    druid_frame_t frame;
    frame.seq = 1;
    frame.cmd = 1;
    frame.len= len;
    frame.p_data = buffer;

    druid_set_construct_trans_frame(frame);
    uint8_t *p_split_frame = NULL;
    do
    {
        printf("\r\n");
        uint16_t split_length;
        p_split_frame = get_split_frame(&split_length);
        if (p_split_frame)
        {
            for (int i = 0; i < split_length; i++)
            {
                printf("%02x ", p_split_frame[i]);
            }
        }
    } while (p_split_frame);
    printf("\r\n");
}