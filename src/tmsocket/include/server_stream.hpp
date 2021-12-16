#ifndef TMSOCKET_SERVER_STREAM_HPP
#define TMSOCKET_SERVER_STREAM_HPP

#include <tmsocket/include/socket_stream.hpp>
#include <prep/include/semaphore.hpp>
#include <prep/include/concurrent_list.hpp>

#include <unordered_set>
#include <thread>
#include <mutex>

TMSOCKET_NAMESPACE_BEGIN

class server_stream : public socket_stream
{
public:

    virtual ~server_stream() noexcept override;

    void
    listen(const ::std::string& host, const ::std::string& port);

    void
    listen(const ::std::string& port)
    {
        listen("", port);
    }

    virtual void
    end_communication() override;

    void
    on_listen(::std::function<void(void)> listen_func)
    {
        this->m_on_listen.subscript(::std::move(listen_func));
    }

    void
    on_connect(::std::function<void(int)> connect_func)
    {
        this->m_on_connect.subscript(::std::move(connect_func));
    }

    void
    send_to_one_client(int client_fd, const ::std::string& msg);

    void
    send_to_all_clients(const ::std::string& msg);

private:

    ::prep::concurrent::event<> m_on_listen;
    ::prep::concurrent::event<int> m_on_connect;
    ::std::mutex m_connect_mtx;
    ::std::unordered_set<int> m_client_fds;
    ::std::mutex m_client_fds_lock;

    void receive_from_client(int client_fd, ::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
    void accept_clients(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
    ::std::shared_ptr<prep::concurrent::semaphore> m_accept_clients_sem;
    ::prep::concurrent::concurrent_list<::std::shared_ptr<prep::concurrent::semaphore>> m_receive_from_client_sems;
};

TMSOCKET_NAMESPACE_END

#endif
