#ifndef PREP_OS_NET_DEFS_H__
#define PREP_OS_NET_DEFS_H__

#include <prep/include/os.h>
#include <stdint.h>

#if defined(_WIN32)

#   if defined(_WIN64)

typedef uint64_t tmsocket_t;

#   else

typedef unsigned int tmsocket_t;

#   endif // _WIN64

#elif defined(__linux__) || defined(__unix__)

typedef int tmsocket_t;

#endif // #if defined(_WIN32)

#endif // #ifndef PREP_OS_NET_DEFS_H__
