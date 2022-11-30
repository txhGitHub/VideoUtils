//
// Created by 18781811841 on 2022/7/19.
//
#include<stdlib.h>
#include <stdio.h>
//#include <sys/types.h>
//#include <unistd.h>
#include "windows.h"


#ifndef VIDEOTOOLS_LOG_H
#define VIDEOTOOLS_LOG_H

#ifndef  __DEBUG
#define LOGFUNC(...) ((void)0)
#else
#define  LOGFUNC(...)  (printf(__VA_ARGS__))
#endif

#define  DEBUG_INFO(format, ...) printf("%d %d Info: %s:%d, %s " format "\n", GetCurrentProcessId(),  \
            GetCurrentThreadId(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);



#endif //VIDEOTOOLS_LOG_H
