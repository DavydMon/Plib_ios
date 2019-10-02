#ifndef LINUX
#include <Windows.h>
#include "dmain.h"
#else


#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "dmain.h"

#ifdef FORCE_CLOCK_MONOTONIC

std::atomic<std::uint32_t> _time_store(GetTickCountRaw());
#endif 

#ifdef CHECK_CLOCK_MONOTONIC

#endif 


#if defined(IOS)
IOS_GET_TICK_COUNT IOS_GetTickCount = 0;

void Set_IOS_GetTickCount(IOS_GET_TICK_COUNT func)
{

    IOS_GetTickCount = func;

}
#endif

std::uint32_t GetCurrentThreadId()
{

    std::uint32_t sid = (std::uint32_t)syscall(SYS_gettid);
    return sid;


}


namespace dmon
{
#endif
    
    char s_space[]=" ";
    char s_point[]=".";

    char s_null[]=    "0";
    char s_true[]=    "true";
    char s_false[]=   "false";
    char s_TRUE[]=    "TRUE";
    char s_FALSE[]=   "FALSE";
    char s_enabled[]= "enabled";
    char s_disabled[]="disabled";

    char s_end[]={13,10,0};

    const char *str_false_true[] = {"false","true"};
    const char *str_off_on[] = {"off","on"};
    const char *str_disabled_enabled[] = {"disabled","enabled"};
    
#ifdef LINUX
}

#endif

bool set_threada_affinity_mask(_dword cpu_affinity_mask,bool rezerved)
{
#ifndef LINUX
    SetThreadAffinityMask(GetCurrentThread(),cpu_affinity_mask);
#elif defined(ANDROID)

#elif defined(WIN)
    cpu_set_t cpuset;
    pthread_t thread;
    int s;
    thread = pthread_self();
    for(_byte i=0;i<32;i++)
    {
        if(cpu_affinity_mask & 0x01)
            CPU_SET(i, &cpuset);
        cpu_affinity_mask >>= 1;
    }
    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
#endif
    return true;
}

