#include "../include/server_communicator.hpp"

#include "details/protocol.ipp"

TMSOCKET_NAMESPACE_BEGIN

server_communicator::server_communicator()
{
    this->m_stm.on_reveive
    (
        [this] (const ::std::string& str)
        {
            this->m_buffer.assign(str);
            ::std::string msg;
            while (protocol::protocol::try_decode_message(this->m_buffer, msg))
            {
                this->m_on_receive.invoke(msg);
            }
        }
    );
}

PREP_NODISCARD bool
server_communicator::is_connected() const
{
    return this->m_stm.is_connected();
}

PREP_NODISCARD bool
server_communicator::is_finished() const
{
    return this->m_stm.is_finished();
}

void
server_communicator::add_log(::std::function<void(const ::std::string&)> log_func)
{
    this->m_stm.add_log(::std::move(log_func));
}

void
server_communicator::on_reveive(::std::function<void(const ::std::string&)> func)
{
    m_on_receive.subscript(::std::move(func));
}

void
server_communicator::listen(const ::std::string& host, const ::std::string& port)
{
    this->m_stm.listen(host, port);
}

void
server_communicator::listen(const ::std::string& port)
{
    this->m_stm.listen(port);
}

void
server_communicator::on_listen(::std::function<void(void)> listen_func)
{
    this->m_stm.on_listen(::std::move(listen_func));
}

void
server_communicator::on_connect(::std::function<void(int)> connect_func)
{
    this->m_stm.on_connect(::std::move(connect_func));
}

void
server_communicator::send_to_one_client(int client_fd, const ::std::string& msg)
{
    this->m_stm.send_to_one_client(client_fd, protocol::protocol::encode_message(msg));
}

void
server_communicator::send_to_all_clients(const ::std::string& msg)
{
    this->m_stm.send_to_all_clients(protocol::protocol::encode_message(msg));
}

void
server_communicator::end_communication()
{
    this->m_stm.end_communication();
}

TMSOCKET_NAMESPACE_END
