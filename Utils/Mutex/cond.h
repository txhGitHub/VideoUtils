#ifndef COND_H
#define COND_H
//考虑到程序的跨平台运行，也为了在程序中代码不难看，通过宏控制mutex, condition的自定义
#ifndef _MSC_VER
#define MINGW
#endif

#if defined(WIN32) && !defined(MINGW)
    #include <WinSock2.h>
    #include <Windows.h>
#else
    #include <pthread.h>
    #include <time.h>
#endif

class Cond
{
public:
    Cond();
    ~Cond();

    int Lock();
    int Unlock();
    int Wait();

    //固定时间等待
    int TimedWait(int second);
    //
    int Signal();

    //唤醒所有睡眠线程
    int Broadcast();

private:
    #if defined(WIN32) && !defined(MINGW)
        CRITICAL_SECTION m_mutex;
        RTL_CONDITION_VARIABLE m_cond;
    #else
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
    #endif
};


#endif // COND_H
