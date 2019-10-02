

#ifndef dmainH
#define dmainH


#define PATH_SEPARATOR_WIN     '\\'
#define PATH_SEPARATOR_STR_WIN "\\"
#define PATH_SEPARATOR_NIX     '/'
#define PATH_SEPARATOR_STR_NIX "/"

#ifdef LINUX

#include <cstdint>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>




#ifdef FORCE_CLOCK_MONOTONIC


#include <atomic>
extern std::atomic<std::uint32_t> _time_store;
#endif 

#ifdef CHECK_CLOCK_MONOTONIC


#endif 

#define GetTickCount() _GetTickCount(__func__, __FILE__, __LINE__)


#if defined(IOS)
typedef std::uint32_t(*IOS_GET_TICK_COUNT)();

extern IOS_GET_TICK_COUNT IOS_GetTickCount;

void Set_IOS_GetTickCount(IOS_GET_TICK_COUNT func);
#endif




inline std::uint32_t GetTickCountRaw()
{
#if defined(IOS)
    if(IOS_GetTickCount)
	return IOS_GetTickCount();
    else
	return 0;
#else
    timespec ts = { 0,0 };
#if defined(CLOCK_MONOTONIC_RAW)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else 
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif 
    return ((std::uint32_t)(ts.tv_sec * 1000)) + ((std::uint32_t)(ts.tv_nsec / 1000000));
#endif
}


inline std::uint32_t _GetTickCount(const char* , const char* , int )
{
    
    std::uint32_t cur_time = GetTickCountRaw();

    

#ifdef FORCE_CLOCK_MONOTONIC
    {
        std::uint32_t prev_time = _time_store; 
        while ((std::int32_t(cur_time - prev_time)) > 0) 
        {
            if (_time_store.compare_exchange_weak(prev_time, cur_time)) 
            {
                prev_time = cur_time; 
                break;
            }
        }
        cur_time = prev_time; 
    }
#endif 

#ifdef CHECK_CLOCK_MONOTONIC
    {
        std::uint32_t prev_time = _time_check.exchange(cur_time);
        if ((std::int32_t(cur_time - prev_time)) < 0)
            printf("Time Back cur_time=%u prev_time=%u\n", cur_time, prev_time);
    }
#endif 

    return cur_time;
}



std::uint32_t GetCurrentThreadId();

namespace dmon {

#endif

#define dPI 3.1415926535897932384626433832795

#ifndef LINUX
	typedef unsigned long       dword;
	typedef unsigned short      word;
	typedef unsigned char       byte;

	typedef unsigned long long  qword;
	typedef long                sdword;

	typedef long long           sqword;

	#define dlib
#else
	typedef std::uint32_t    dword;
	typedef std::uint16_t    word;
	typedef std::uint8_t     byte;
	typedef std::uint64_t    qword;
	typedef std::int32_t     sdword;
	typedef std::int64_t     sqword;
	typedef std::uintptr_t   uintptr;

	#define dlib dmon::
#endif

extern char s_space[];
extern char s_point[];

extern char s_null[];
extern char s_true[];
extern char s_false[];
extern char s_TRUE[];
extern char s_FALSE[];
extern char s_enabled[];
extern char s_disabled[];

extern char s_end[];

extern const char *str_false_true[2];
extern const char *str_off_on[2];
extern const char *str_disabled_enabled[2];

#define _strl(str) str,(sizeof(str) - 1)
#define _strls(str) { str, (sizeof(str) - 1) }
#define _sizeof(data) data,sizeof(data)
#define _sizeofs(data) { data,sizeof(data) }

#define _str_false_true(value)        str_false_true[value & 0x01]
#define _str_off_on(value)            str_off_on[value & 0x01]
#define _str_disabled_enabled(value)  str_disabled_enabled[value & 0x01]


#ifdef LINUX
}

#define PATH_SEPARATOR     PATH_SEPARATOR_NIX
#define PATH_SEPARATOR_STR PATH_SEPARATOR_STR_NIX
#else
#define PATH_SEPARATOR     PATH_SEPARATOR_NIX
#define PATH_SEPARATOR_STR PATH_SEPARATOR_STR_NIX
#endif

#ifndef LINUX
typedef dword  _dword;
typedef word   _word;
typedef byte   _byte;
typedef qword  _qword;
typedef sdword _sdword;
typedef sqword _sqword;
#else
#define _dword  dmon::dword
#define _word   dmon::word
#define _byte   dmon::byte
#define _qword  dmon::qword
#define _sdword dmon::sdword
#define _sqword dmon::sqword
#endif

bool set_threada_affinity_mask(_dword cpu_affinity_mask,bool rezerved=false);


#endif
