#ifndef TMSOCKET_CLIENT_STREAM_HPP
#define TMSOCKET_CLIENT_STREAM_HPP

#include <prep/include/concurrent_queue.hpp>
#include <prep/include/semaphore.hpp>
#include <tmsocket/include/socket_stream.hpp>
#include <mutex>
#include <thread>
#include <memory>

TMSOCKET_NAMESPACE_BEGIN

class client_stream final : public socket_stream
{
public:
    virtual
    ~client_stream() noexcept;

    virtual void
    end_communication() override;

    void
    connect(const ::std::string& host, const ::std::string& port);

    void
    on_connect(::std::function<void(void)> connect_func)
    {
        this->m_on_connect.subscript(::std::move(connect_func));
    }

    void
    send_to_server(const ::std::string& msg);

private:
    ::prep::concurrent::event<> m_on_connect;
    ::std::mutex m_connect_mtx;
    // ::std::unique_ptr<::std::thread> m_thrd_recv_from_server;
    ::std::shared_ptr<::prep::concurrent::semaphore> m_receive_from_server_sem;

    void
    receive_from_server(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept;
};

TMSOCKET_NAMESPACE_END

#endif
