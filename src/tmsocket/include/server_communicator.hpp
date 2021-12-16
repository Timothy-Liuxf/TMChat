#ifndef TMSOCKET_SERVER_COMMUNICATOR_HPP__
#define TMSOCKET_SERVER_COMMUNICATOR_HPP__

#include <prep/include/event.hpp>
#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/server_stream.hpp>

#include <utility>
#include <functional>
#include <map>

TMSOCKET_NAMESPACE_BEGIN

class server_communicator
{
public:

    server_communicator();
    server_communicator(const server_communicator&) = delete;
    server_communicator& operator=(const server_communicator&) = delete;
    ~server_communicator() noexcept = default;

    PREP_NODISCARD bool
    is_connected() const;

    PREP_NODISCARD bool
    is_finished() const;

    void
    add_log(::std::function<void(const ::std::string&)> log_func);

    void
    on_reveive(::std::function<void(int, const ::std::string&)> func);

    void
    listen(const ::std::string& host, const ::std::string& port);

    void
    listen(const ::std::string& port);

    void
    on_listen(::std::function<void(void)> listen_func);

    void
    on_connect(::std::function<void(int)> connect_func);

    void
    on_disconnect(::std::function<void(int)> disconnect_func);

    void
    send_to_one_client(int client_fd, const ::std::string& msg);

    void
    send_to_all_clients(const ::std::string& msg);
    
    void
    end_communication();

private:
    server_stream m_stm;
    ::std::map<int, ::std::string> m_buffers;
    ::prep::concurrent::event<int, const ::std::string&> m_on_receive;
};

TMSOCKET_NAMESPACE_END

#endif // #ifndef TMSOCKET_SERVER_COMMUNICATOR_HPP__
