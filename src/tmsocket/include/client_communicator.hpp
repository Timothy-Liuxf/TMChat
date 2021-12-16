#ifndef TMSOCKET_CLIENT_COMMUNICATOR_HPP__
#define TMSOCKET_CLIENT_COMMUNICATOR_HPP__

#include <prep/include/event.hpp>
#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/client_stream.hpp>

#include <utility>
#include <functional>

TMSOCKET_NAMESPACE_BEGIN

class client_communicator
{
public:

    client_communicator();
    client_communicator(const client_communicator&) = delete;
    client_communicator& operator=(const client_communicator&) = delete;
    ~client_communicator() noexcept = default;

    PREP_NODISCARD bool
    is_connected() const;

    PREP_NODISCARD bool
    is_finished() const;

    void
    add_log(::std::function<void(const ::std::string&)> log_func);

    void
    on_reveive(::std::function<void(const ::std::string&)> func);

    void
    connect(const ::std::string& host, const ::std::string& port);

    void
    on_connect(::std::function<void(void)> listen_func);

    void
    send_to_server(const ::std::string& msg);
    
    void
    end_communication();

private:
    client_stream m_ctm;
    ::std::string m_buffer;
    ::prep::concurrent::event<const ::std::string&> m_on_receive;
};

TMSOCKET_NAMESPACE_END

#endif // #ifndef TMSOCKET_CLIENT_COMMUNICATOR_HPP__ // #ifndef TMSOCKET_CLIENT_COMMUNICATOR_HPP__
