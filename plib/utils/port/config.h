
#pragma once

#include "utils/port/platform.h"



#if PL_WINDOWS
  #define PL_PORT_WINDOWS 1
#else
  #define PL_PORT_POSIX 1
#endif

#if PL_LINUX || PL_ANDROID || PL_TIZEN
  #define PL_POLL_EPOLL 1
  #define PL_EVENTFD_LINUX 1
#elif PL_CYGWIN
  #define PL_POLL_SELECT 1
  #define PL_EVENTFD_BSD 1
#elif PL_EMSCRIPTEN
  #define PL_POLL_POLL 1
  #define PL_EVENTFD_UNSUPPORTED 1
#elif PL_DARWIN
  #define PL_POLL_KQUEUE 1
  #define PL_EVENTFD_BSD 1
#elif PL_WINDOWS
  #define PL_POLL_WINEVENT 1
  #define PL_EVENTFD_WINDOWS 1
#else
  #error "Poll's implementation is not defined"
#endif

#if PL_EMSCRIPTEN
  #define PL_THREAD_UNSUPPORTED 1
#elif PL_TIZEN
  #define PL_THREAD_PTHREAD 1
#else
  #define PL_THREAD_STL 1
#endif


