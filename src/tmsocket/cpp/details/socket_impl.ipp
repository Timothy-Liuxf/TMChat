#if !defined(TMSOCKET_SERVER_STREAM_HPP__) && !defined(TMSOCKET_CLIENT_STREAM_HPP__)
#   error Please include <tmsocket/include/server_communicator.hpp> or <tmsocket/include/client_communicator.hpp> instead.
#endif

#ifndef TMSOCKET_DETAILS_SOCKET_IMPL_IPP__
#define TMSOCKET_DETAILS_SOCKET_IMPL_IPP__

PREP_NODISCARD static int
connect_impl(const ::std::string& host, const ::std::string& port, socket_stream* stm, int (*connect_func)(int, const sockaddr*, socklen_t))
{
    if (stm->is_finished())
    {
        throw ::std::logic_error("Communication finished!");
    }

    if (stm->is_connected())
    {
        throw ::std::logic_error("Already connected!");
    }

    int ret = 0;

    // Init parameters

    addrinfo hints;
    ::std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
    addrinfo* addrlist;
    ret = ::getaddrinfo(host == "" ? nullptr : host.c_str(),
                port == "" ? nullptr : port.c_str(),
                &hints, &addrlist);
    
    if (ret != 0)
    {
        throw invalid_input("Illegal address!", ret);
    }

    // connect

    int fd;
    bool socket_inited = false;
    prep::errno_type socket_errno = 0, connect_errno = 0;
    try
    {
        for (auto p = addrlist; p != nullptr; p = p->ai_next)
        {
            fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (fd < 0)
            {
                socket_errno = get_net_error();
                continue;
            }
            socket_inited = true;

            ret = connect_func(fd, p->ai_addr, p->ai_addrlen);
            if (ret == 0) break;

            connect_errno = get_net_error();
            ::close_socket(fd);
            fd = -1;
        }
    }
    catch(...)
    {
        ::freeaddrinfo(addrlist);
        throw;
    }
    ::freeaddrinfo(addrlist);

    if (fd < 0)
    {
        if (!socket_inited)
        {
            throw fail_to_init_socket("Fail to init socket!", socket_errno);
        }
        else
        {
            throw fail_to_connect("Fail to connect!", connect_errno);
        }
    }

    return fd;
}

static void
send_msg(int fd, const ::std::string& msg, socket_stream* stm)
{
    ::std::size_t has_sent = 0;
    while (!stm->is_finished() && has_sent < msg.length())
    {
        int sz = send(fd, msg.c_str() + has_sent, msg.length() - has_sent, 0);
        if (sz == -1)
        {
            throw fail_to_send("Fail to send message!", get_net_error());
        }
        has_sent += sz;
    }
}

#endif // #ifndef TMSOCKET_DETAILS_SOCKET_IMPL_IPP__
