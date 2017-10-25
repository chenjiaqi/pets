#ifndef USER_LOG_H
#define USER_LOG_H

#ifdef DEBUG_MODE
#define LOG_PROC(log_level, format, arguments...) \
    printf("[%s] ", log_level);    \
    printf(format, ##arguments);    \
    printf("\n");
#else
    #define LOG_PROC(log_level, format, arguments...) 
#endif



#endif