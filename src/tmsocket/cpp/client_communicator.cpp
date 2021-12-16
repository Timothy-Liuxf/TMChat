#include "../include/client_communicator.hpp"

#include "details/protocol.ipp"

TMSOCKET_NAMESPACE_BEGIN

client_communicator::client_communicator()
{
    this->m_ctm.on_reveive
    (
        [this] (const ::std::string& str)
        {
            this->m_buffer.append(str);
            ::std::string msg;
            while (protocol_ns::protocol::try_decode_message(this->m_buffer, msg))
            {
                this->m_on_receive.invoke(msg);
            }
        }
    );
}

PREP_NODISCARD bool
client_communicator::is_connected() const
{
    return this->m_ctm.is_connected();
}

PREP_NODISCARD bool
client_communicator::is_finished() const
{
    return this->m_ctm.is_finished();
}

void
client_communicator::add_log(::std::function<void(const ::std::string&)> log_func)
{
    this->m_ctm.add_log(::std::move(log_func));
}

void
client_communicator::on_reveive(::std::function<void(const ::std::string&)> func)
{
    m_on_receive.subscript(::std::move(func));
}

void
client_communicator::connect(const ::std::string& host, const ::std::string& port)
{
    this->m_ctm.connect(host, port);
}

void
client_communicator::on_connect(::std::function<void(void)> connect_func)
{
    this->m_ctm.on_connect(::std::move(connect_func));
}

void
client_communicator::send_to_server(const ::std::string& msg)
{
    this->m_ctm.send_to_server(protocol_ns::protocol::encode_message(msg));
}

void
client_communicator::end_communication()
{
    this->m_ctm.end_communication();
}

TMSOCKET_NAMESPACE_END
