#ifndef PREP_OS_NET_DEFS_H__
#define PREP_OS_NET_DEFS_H__

#include <prep/include/os.h>
#include <stdint.h>



#if defined(PREP_WINDOWS)


// define the type of socket

#   if defined(PREP_WINDOWS64)
typedef uint64_t tmsocket_t;
#   else
typedef unsigned int tmsocket_t;
#   endif // _WIN64



#elif defined(PREP_LINUX) || defined(PREP_UNIX)


// define the type of socket

typedef int tmsocket_t;



#endif // #if defined(PREP_WINDOWS)



#endif // #ifndef PREP_OS_NET_DEFS_H__
