#include "../include/server_stream.hpp"
#include "../include/client_stream.hpp"

#include <prep/include/os_net.h>
#include <cstring>
#include <stdexcept>

TMSOCKET_NAMESPACE_BEGIN

// server_stream

server_stream::~server_stream() noexcept
{
    try
    {
        this->end_communication();
    }
    catch(...)
    {
        // No operation
    }
    
}

void
server_stream::end_communication()
{
    bool org_val = m_is_finished.exchange(true);
    if (!org_val)
    {
        this->m_msg_q.emplace(msg_type::finish, "");
        this->wait_for_finish_pick();
        ::close_socket(this->m_fd);
        this->m_fd = -1;
        this->m_thrd_accept_clients->join();
    }
}

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

void
server_stream::receive_from_client(int client_fd) noexcept
{
    try
    {
        char* buf = new char[this->buf_size()];
        while (!this->is_finished())
        {
            int len = ::recv(client_fd, buf, this->buf_size(), 0);
            if (len < 0)
            {
                if (!this->is_finished()) this->m_msg_q.emplace(msg_type::error, "Fail to receive message!");
            }
            else if (len == 0)
            {
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
                    this->m_client_fds.erase(client_fd);
                }
                this->m_msg_q.emplace(msg_type::log, "A client closed the connection!");
                break;
            }
            else
            {
                this->m_msg_q.emplace(msg_type::msg, ::std::string(buf, len));
            }
        }
        delete[] buf;
    }
    catch (::std::bad_alloc&)
    {
        this->m_msg_q.emplace(msg_type::critical_error, "Cannot allocate buffer!");
    }
}

void
server_stream::accept_clients() noexcept
{
    try
    {
        ::std::vector<::std::unique_ptr<::std::thread>> thrds;
        try
        {
            while (!this->is_finished())
            {
                sockaddr clientaddr;
                socklen_t clientaddr_len = sizeof(clientaddr);
                ::std::memset(&clientaddr, 0, sizeof(clientaddr));
                int client_fd = ::accept(this->m_fd, &clientaddr, &clientaddr_len);
                if (client_fd < 0)
                {
                    if (!this->is_finished()) this->m_msg_q.emplace(msg_type::error, netexcept{"Accept client failed!", get_net_error()}.what());
                }
                else
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
                    this->m_client_fds.insert(client_fd);
                    this->m_on_connect.invoke(client_fd);

                    thrds.emplace_back(new ::std::thread{ &server_stream::receive_from_client, this, client_fd });
                }
            }

            for (auto&& thrd : thrds)
            {
                thrd->join();
            }
        }
        catch (...)
        {
            for (auto&& thrd : thrds)
            {
                thrd->join();
            }
            throw;
        }
        
    }
    catch (::std::exception& e)
    {
        this->m_msg_q.emplace(msg_type::critical_error, e.what());
    }
}

void
server_stream::listen(const ::std::string& host, const ::std::string& port)
{
    {
        ::std::unique_lock<::std::mutex> lock(this->m_connect_mtx);
        this->m_fd = connect_impl(host, port, this, &::bind);

        try
        {
            int ret;
            ret = ::listen(this->m_fd, 1024);
            if (ret != 0)
            {
                throw fail_to_listen("Fail to listen!", get_net_error());
            }
        }
        catch(...)
        {
            ::close_socket(this->m_fd);
            this->m_fd = -1;
            throw;
        }

        this->m_is_connected = true;
    }
    
    this->m_on_listen.invoke();
    this->m_thrd_accept_clients.reset(new ::std::thread(&server_stream::accept_clients, this));

    this->pick_msg();
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

void
server_stream::send_to_one_client(int client_fd, const ::std::string& msg)
{
    if (!this->is_connected())
    {
        throw ::std::logic_error("Hasn't connected!");
    }
    if (this->is_finished())
    {
        throw ::std::logic_error("Communication has finished!");
    }
    
    {
        ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
        if (this->m_client_fds.find(client_fd) == this->m_client_fds.end())
        {
            throw client_not_exist(client_fd);
        }
        send_msg(client_fd, msg, this);
    }
}

void
server_stream::send_to_all_clients(const ::std::string& msg)
{
    ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
    for (auto client_fd : this->m_client_fds)
    {
        try
        {
            send_msg(client_fd, msg, this);
        }
        catch (...)
        {
            // No operation
        }
    }
}

// client_stream

client_stream::~client_stream() noexcept
{
    try
    {
        this->end_communication();
    }
    catch(...)
    {
        // No operation
    }
    
}

void
client_stream::receive_from_server() noexcept
{
    try
    {
        char* buf = new char[this->buf_size()];
        while (!this->is_finished())
        {
            int len = ::recv(this->m_fd, buf, this->buf_size(), 0);
            if (len < 0)
            {
                if (!this->is_finished()) this->m_msg_q.emplace(msg_type::critical_error, "Fail to receive message!");
            }
            else if (len == 0)
            {
                this->end_communication();
            }
            else
            {
                this->m_msg_q.emplace(msg_type::msg, ::std::string(buf, len));
            }
        }
        delete[] buf;
    }
    catch (::std::bad_alloc&)
    {
        this->m_msg_q.emplace(msg_type::critical_error, "Cannot allocate buffer!");
    }
}

void
client_stream::connect(const ::std::string& host, const ::std::string& port)
{
    {
        ::std::unique_lock<::std::mutex> lock(this->m_connect_mtx);
        this->m_fd = connect_impl(host, port, this, &::connect);
        this->m_is_connected = true;
    }
    this->m_on_connect.invoke();

    this->m_thrd_recv_from_server.reset(new ::std::thread(&client_stream::receive_from_server, this));
    this->pick_msg();
}

void
client_stream::send_to_server(const ::std::string& msg)
{
    send_msg(this->m_fd, msg, this);
}

void
client_stream::end_communication()
{
    bool org_val = m_is_finished.exchange(true);
    if (!org_val)
    {
        this->m_msg_q.emplace(msg_type::finish, "");
        this->wait_for_finish_pick();
        ::close_socket(this->m_fd);
        this->m_fd = -1;
        this->m_thrd_recv_from_server->join();
    }
}

TMSOCKET_NAMESPACE_END
