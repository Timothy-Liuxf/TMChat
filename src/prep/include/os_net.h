#ifndef PREP_OS_NET_H__
#define PREP_OS_NET_H__

#include <prep/include/os.h>
#include <prep/include/os_net_defs.h>

#if defined(PREP_WINDOWS)

#   include <WinSock2.h>
#   include <ws2tcpip.h>

#   define get_net_error()                  WSAGetLastError()
#   define close_socket(fd)                 closesocket(fd)
#   define invalid_socket(fd)               ((fd) == INVALID_SOCKET)
#   define socket_shutdown(return_by_recv)  ((return_by_recv) == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
#   define receive_error(return_by_recv)    ((return_by_recv) == SOCKET_ERROR && WSAGetLastError() != WSAECONNRESET)

#elif defined(PREP_LINUX) || defined(PREP_UNIX)

#   include <sys/socket.h>
#   include <netdb.h>

#   define get_net_error() (errno)
#   define close_socket(fd) close(fd)
#   define invalid_socket(fd) ((fd) < 0)
#   define socket_shutdown(return_by_recv)  ((return_by_recv) == 0)
#   define receive_error(return_by_recv)    ((return_by_recv) < 0)

#endif // #if defined(_WIN32)

#endif // #ifndef PREP_OS_NET_H__
