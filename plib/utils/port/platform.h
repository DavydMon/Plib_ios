
#pragma once




#if defined(_WIN32)
  #if defined(__cplusplus_winrt)
    #define PL_WINRT 1
  #endif
  #if defined(__cplusplus_cli)
    #define PL_CLI 1
  #endif
  #define PL_WINDOWS 1
#elif defined(__APPLE__)
  #include "TargetConditionals.h"
  #if TARGET_OS_IPHONE
    
    #if TARGET_OS_IOS
      #define PL_DARWIN_IOS 1
    #elif TARGET_OS_TV
      #define PL_DARWIN_TV_OS 1
    #elif TARGET_OS_WATCH
      #define PL_DARWIN_WATCH_OS 1
    #else
      #warning "Probably unsupported Apple iPhone platform. Feel free to try to compile"
    #endif
  #elif TARGET_OS_MAC
    
    #define PL_DARWIN_MAC 1
  #else
    #warning "Probably unsupported Apple platform. Feel free to try to compile"
  #endif
  #define PL_DARWIN 1
#elif defined(ANDROID) || defined(__ANDROID__)
  #define PL_ANDROID 1
#elif defined(TIZEN_DEPRECATION)
  #define PL_TIZEN 1
#elif defined(__linux__)
  #define PL_LINUX 1
#elif defined(__CYGWIN__)
  #define PL_CYGWIN 1
#elif defined(__EMSCRIPTEN__)
  #define PL_EMSCRIPTEN 1
#elif defined(__unix__)  
  #warning "Probably unsupported Unix platform. Feel free to try to compile"
  #define PL_CYGWIN 1
#else
  #error "Probably unsupported platform. Feel free to remove the error and try to recompile"
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER)
  #define PL_INTEL 1
#elif defined(__clang__)
  #define PL_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
  #define PL_GCC 1
#elif defined(_MSC_VER)
  #define PL_MSVC 1
#else
  #warning "Probably unsupported compiler. Feel free to try to compile"
#endif

#if PL_GCC || PL_CLANG || PL_INTEL
  #define PL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
  #define PL_ATTRIBUTE_FORMAT_PRINTF(from, to) __attribute__((format(printf, from, to)))
#else
  #define PL_WARN_UNUSED_RESULT
  #define PL_ATTRIBUTE_FORMAT_PRINTF(from, to)
#endif

#if PL_MSVC
  #define PL_UNUSED __pragma(warning(suppress : 4100))
#elif PL_CLANG || PL_GCC || PL_INTEL
  #define PL_UNUSED __attribute__((unused))
#else
  #define PL_UNUSED
#endif

#define PL_HAVE_ATOMIC_SHARED_PTR 1



#ifdef __GLIBCXX__
  #undef PL_HAVE_ATOMIC_SHARED_PTR
#endif


#if defined(__has_feature)
  #if !__has_feature(cxx_atomic)
    #undef PL_HAVE_ATOMIC_SHARED_PTR
  #endif
#endif

#ifdef PL_HAVE_ATOMIC_SHARED_PTR 
  #undef PL_HAVE_ATOMIC_SHARED_PTR
#endif

#define PL_CONCURRENCY_PAD 128


