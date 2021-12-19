#ifndef TMSOCKET_CLIENT_STREAM_HPP__
#define TMSOCKET_CLIENT_STREAM_HPP__

#include <prep/include/prep.h>
#include <prep/include/concurrent_queue.hpp>
#include <prep/include/semaphore.hpp>
#include <prep/include/concurrent_queue.hpp>
#include <prep/include/os.h>
#include <prep/include/os_net_defs.h>
#include <prep/include/event.hpp>
#include <tmsocket/include/defs.hpp>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>

TMSOCKET_NAMESPACE_BEGIN

#include "details/socket_stream.ipp"

class client_stream : private socket_stream
{
public:

    using socket_stream::buf_size;
    using socket_stream::is_connected;
    using socket_stream::is_finished;
    using socket_stream::add_log;

    virtual
    ~client_stream() noexcept;

    void
    end_communication();

    void
    connect(const ::std::string& host, const ::std::string& port);

    void
    on_connect(::std::function<void(void)> connect_func)
    {
        this->m_on_connect.subscript(::std::move(connect_func));
    }
    
    void
    on_reveive(::std::function<void(const ::std::string&)> func)
    {
        m_on_receive.subscript(::std::move(func));
    }

    void
    send_to_server(const ::std::string& msg);

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
        disconnect_unexpectly = 5
    };

    ::prep::concurrent::event<> m_on_connect;
    ::prep::concurrent::event<const ::std::string&> m_on_receive;
    ::std::mutex m_connect_mtx;
    ::std::shared_ptr<::prep::concurrent::semaphore> m_receive_from_server_sem;

    ::std::atomic_bool m_begin_pick_msg { false };
    bool m_finish_pick = false;
    ::prep::concurrent::concurrent_queue<::std::pair<msg_type, ::std::string>> m_msg_q;
    mutable ::std::condition_variable m_finish_pick_cond;
    mutable ::std::mutex m_finish_pick_mtx;

    void
    receive_from_server(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
};

TMSOCKET_NAMESPACE_END

#endif
