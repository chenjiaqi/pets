#ifndef USER_LOG_H
#define USER_LOG_H

#include <stdio.h>
#ifdef DEBUG_MODE

#define LOG_UINT(a) ((unsigned int)a)

#define PRE_INFO "INFO"

#define LOG_PROC(log_level, format, arguments...) \
    printf("[%s] ", log_level);    \
    printf(format, ##arguments);    \
    printf("\r\n");

#define LOG_INFO(format, args...) LOG_PROC("INFO", format, ##args)
#define LOG_ERROR(format, args...) LOG_PROC("ERROR", format, ##args)
#define LOG_EVENT(format, args...) LOG_PROC("EVENT", format, ##args)


#else
    #define LOG_PROC(log_level, format, arguments...) 
    #define LOG_INFO(format, args...)
    #define LOG_ERROR(format, args...)
    #define LOG_EVENT(format, args...)
#endif



#endif