#ifndef TMSOCKET_SOCKET_STREAM_HPP__
#define TMSOCKET_SOCKET_STREAM_HPP__

#include <prep/include/event.hpp>
#include <prep/include/concurrent_queue.hpp>
#include <prep/include/os.h>
#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/netexcept.hpp>
#include <string>
#include <atomic>
#include <functional>
#include <utility>
#include <memory>
#include <mutex>
#include <condition_variable>

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

    socket_stream(int buf_size = 128)
                : m_fd(-1),
                  m_is_finished(false),
                  m_is_connected(false),
                  m_buf_size(buf_size) {}

    PREP_NODISCARD int
    buf_size() const noexcept
    {
        return this->m_buf_size;
    }

    virtual
    ~socket_stream() noexcept {}

    PREP_NODISCARD bool
    is_connected() const
    {
        return this->m_is_connected;
    }

    PREP_NODISCARD bool
    is_finished() const
    {
        return this->m_is_finished;
    }

    void
    add_log(::std::function<void(const ::std::string&)> log_func)
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

    void
    pick_msg();

    void
    wait_for_finish_pick() const;

private:
    bool m_finish_pick = false;
    mutable ::std::condition_variable m_finish_pick_cond;
    mutable ::std::mutex m_finish_pick_mtx;
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

#endif // #ifndef TMSOCKET_SOCKET_STREAM_HPP__
