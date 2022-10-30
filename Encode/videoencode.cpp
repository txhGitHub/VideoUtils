#include "videoencode.h"
#include <stdio.h>
#include "log.h"
#pragma execution_character_set("utf-8")

VideoEncode::VideoEncode()
{
    int num = 0;
    char str[20] = "lll";
    //打印结果在缓冲区未被释放，
    DEBUG_INFO("test %d %s", num, str);
    fflush(stdout);
}
