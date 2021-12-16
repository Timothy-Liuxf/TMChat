#ifndef TMSOCKET_DETAILS_SOCKET_STREAM_IPP__
#define TMSOCKET_DETAILS_SOCKET_STREAM_IPP__

#include <prep/include/event.hpp>
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

protected:
    
    int m_fd;
    ::std::atomic_bool m_is_finished;
    ::std::atomic_bool m_is_connected;
    const int m_buf_size;
    ::std::string m_buf;
    ::prep::concurrent::event<const ::std::string&> m_logger;
};

TMSOCKET_NAMESPACE_END

#endif // #ifndef TMSOCKET_DETAILS_SOCKET_STREAM_IPP__
