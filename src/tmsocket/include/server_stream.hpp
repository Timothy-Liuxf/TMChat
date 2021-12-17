#ifndef TMSOCKET_SERVER_STREAM_HPP__
#define TMSOCKET_SERVER_STREAM_HPP__

#include <prep/include/semaphore.hpp>
#include <prep/include/concurrent_list.hpp>
#include <prep/include/concurrent_queue.hpp>
#include <prep/include/os.h>
#include <prep/include/os_net_defs.h>
#include <prep/include/event.hpp>
#include <tmsocket/include/defs.hpp>

#include <unordered_set>
#include <thread>
#include <mutex>
#include <atomic>
#include <type_traits>

TMSOCKET_NAMESPACE_BEGIN

#include "details/socket_stream.ipp"

class server_stream : private socket_stream
{
public:

    using socket_stream::buf_size;
    using socket_stream::is_connected;
    using socket_stream::is_finished;
    using socket_stream::add_log;

    virtual ~server_stream() noexcept override;

    void
    listen(const ::std::string& host, const ::std::string& port);

    void
    listen(const ::std::string& port)
    {
        this->listen("", port);
    }

    void
    end_communication();

    void
    on_listen(::std::function<void(void)> listen_func)
    {
        this->m_on_listen.subscript(::std::move(listen_func));
    }

    void
    on_connect(::std::function<void(tmsocket_t)> connect_func)
    {
        this->m_on_connect.subscript(::std::move(connect_func));
    }

    void
    on_disconnect(::std::function<void(tmsocket_t)> disconnect_func)
    {
        this->m_on_disconnect.subscript(::std::move(disconnect_func));
    }

    void
    on_reveive(::std::function<void(tmsocket_t, const ::std::string&)> func)
    {
        m_on_receive.subscript(::std::move(func));
    }

    void
    send_to_one_client(tmsocket_t client_fd, const ::std::string& msg);

    void
    send_to_all_clients(const ::std::string& msg);

    void
    pick_msg();

    void
    wait_for_finish_pick() const;

private:

    enum class msg_type
    {
        error = 0,
        critical_error = 1,
        log = 2,
        msg = 3,
        finish = 4,
        disconnect = 5
    };

    ::prep::concurrent::event<> m_on_listen;
    ::prep::concurrent::event<tmsocket_t> m_on_connect;
    ::prep::concurrent::event<tmsocket_t> m_on_disconnect;
    ::prep::concurrent::event<tmsocket_t, const ::std::string&> m_on_receive;
    ::std::mutex m_connect_mtx;
    ::std::unordered_set<tmsocket_t> m_client_fds;
    ::std::mutex m_client_fds_lock;

    void receive_from_client(tmsocket_t client_fd, ::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
    void accept_clients(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
    ::std::shared_ptr<prep::concurrent::semaphore> m_accept_clients_sem;
    ::prep::concurrent::concurrent_list<::std::shared_ptr<prep::concurrent::semaphore>> m_receive_from_client_sems;

    bool m_finish_pick = false;
    ::prep::concurrent::concurrent_queue<::std::pair<msg_type, ::std::pair<int64_t, ::std::string>>> m_msg_q;
    mutable ::std::condition_variable m_finish_pick_cond;
    mutable ::std::mutex m_finish_pick_mtx;
};

TMSOCKET_NAMESPACE_END

#endif
