#ifndef SOCKET_STREAM_HPP__
#define SOCKET_STREAM_HPP__

#include <prep/include/event.hpp>
#include <prep/include/concurrent_queue.hpp>
#include <prep/include/os.h>
#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/netexcept.hpp>
#include <string>
#include <atomic>
#include <utility>
#include <memory>
#include <unordered_set>
#include <thread>

TMSOCKET_NAMESPACE_BEGIN

class socket_stream
{
public:

    enum class msg_type
    {
        error = 0,
        critical_error = 1,
        log = 2,
        msg = 3,
        finish = 4
    };

    socket_stream(int buf_size = 128);

    PREP_NODISCARD
    int
    buf_size() const noexcept
    { return this->m_buf_size; }

    virtual
    ~socket_stream() noexcept = 0;

    PREP_NODISCARD
    bool
    is_connected() const;

    PREP_NODISCARD
    bool
    is_finished() const;

    void
    add_log(::std::function<void(const ::std::string)> log_func)
    {
        m_logger.subscript(::std::move(log_func));
    }

    void
    on_reveive(::std::function<void(const ::std::string&)> func)
    {
        m_on_receive.subscript(::std::move(func));
    }

    virtual void
    end_communication() = 0;

protected:
    
    int m_fd;
    ::std::atomic_bool m_is_finished;
    ::std::atomic_bool m_is_connected;
    const int m_buf_size;
    ::std::string m_buf;
    ::prep::concurrent::concurrent_queue<::std::pair<msg_type, ::std::string>> m_msg_q;
    ::prep::concurrent::event<const ::std::string&> m_logger;
    ::prep::concurrent::event<const ::std::string&> m_on_receive;
};

class server_stream final : public socket_stream
{
public:

    virtual ~server_stream() noexcept override;

    void
    listen(const ::std::string& host, const ::std::string& port);

    virtual void
    end_communication() override;

    void
    on_listen(::std::function<void(void)> connect_func)
    {
        this->m_on_listen.subscript(::std::move(connect_func));
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

    void receive_from_client(int client_fd) noexcept;
    void accept_clients() noexcept;
    void pick_msg();
    ::std::unique_ptr<::std::thread> m_thrd_accept_clients;
};


class fail_to_init_socket : public netexcept
{
public:
    fail_to_init_socket(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_connect : public netexcept
{
public:
    fail_to_connect(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_listen : public netexcept
{
public:
    fail_to_listen(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_send : public netexcept
{
public:
    fail_to_send(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};


class client_not_exist : public tmsocket_exception
{
public:
    client_not_exist(int fd) : m_fd(fd) {}

    PREP_NODISCARD
    virtual const char*
    what() const noexcept override
    {
        return "The client specified doesn't exists!";
    }

    PREP_NODISCARD
    int
    get_fd() const noexcept
    {
        return this->m_fd;
    }

private:
    int m_fd;
};



TMSOCKET_NAMESPACE_END

#endif
