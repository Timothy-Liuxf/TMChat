#ifndef PREP_OS_H__
#define PREP_OS_H__

#include <prep/include/namespace.hpp>

#if defined(_WIN32)
#   define PREP_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>

#   define SYSCALL __stdcall

PREP_NAMESPACE_BEGIN

using errno_type = int;

PREP_NAMESPACE_END

#elif defined(__linux__) || defined(__unix__)
#   define PREP_LINUX
#   define PREP_UNIX
#   include <unistd.h>
#   include <errno.h>

#   define SYSCALL

PREP_NAMESPACE_BEGIN

using errno_type = error_t;

PREP_NAMESPACE_END

#else
#   error This platform is not supported. Please use UNIX-like operating systems or Windows.
#endif // #if defined(_WIN32)

#endif
