#ifndef PREP_OS_NET_H__
#define PREP_OS_NET_H__

#include <prep/include/os.h>

#if defined(_WIN32)

#   include <WinSock2.h>

#   define get_net_error() ::WSAGetLastError()

#elif defined(__linux__) || defined(__unix__)

#   include <sys/socket.h>
#   include <netdb.h>

#   define get_net_error() (errno)

#endif // #if defined(_WIN32)

#endif // #ifndef PREP_OS_NET_H__
