#ifndef PREP_OS_H__
#define PREP_OS_H__

#include <prep/include/namespace.hpp>

#if defined(_WIN32)



#   define PREP_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>

#   if defined(_WIN64)
#       define PREP_WINDOWS64
#   endif

#   define PREP_SYSCALL __stdcall

PREP_NAMESPACE_BEGIN

using errno_type = int;

PREP_NAMESPACE_END



#elif defined(__linux__) || defined(__unix__)   // defined(_WIN32)

#   ifdef __linux__
#       define PREP_LINUX
#   endif   //  ifdef __linux__

#   ifdef __unix__
#       define PREP_UNIX
#   endif   // ifdef __unix__


#   include <unistd.h>
#   include <errno.h>

#   define PREP_SYSCALL

PREP_NAMESPACE_BEGIN

using errno_type = error_t;

PREP_NAMESPACE_END

#elif defined(__APPLE__) || defined(__MACH__)   // defined(_WIN32)

#   define PREP_APPLE
#   include <unistd.h>
#   include <errno.h>

#   define PREP_SYSCALL

PREP_NAMESPACE_BEGIN

using errno_type = int;

PREP_NAMESPACE_END

#else
#   error This platform is not supported. Please use UNIX-like operating systems or Windows.



#endif // #if defined(_WIN32)

#endif
