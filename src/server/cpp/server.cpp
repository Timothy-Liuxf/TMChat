#include "../include/server.hpp"

#include <iostream>
#include <sstream>
#include <type_traits>
#include "../../common/protocol.hpp"

using ::std::cout;
using ::std::cerr;
using ::std::cin;
using ::std::endl;

TMCHAT_NAMESPACE_BEGIN

server::
server(create_database_t create_database) : m_database_ptr(create_database())
{
    this->m_communicator.on_listen
    (
        []
        {
            cout << "Successfully connected!" << endl;
        }
    );
    this->m_communicator.add_log([](const ::std::string& str) { cout << str << endl; });

    this->m_communicator.on_connect([](tmsocket_t fd) { cout << "A client: " << fd << " connected!" << endl; });
    this->m_communicator.on_reveive([this](tmsocket_t fd, const ::std::string& msg) { this->on_receive(fd, msg); });
    this->m_communicator.on_disconnect([this](tmsocket_t fd) { this->on_disconnect(fd); });

    cout << "Please input server port: " << std::flush;
    ::std::string port;
    ::std::getline(cin, port);

    ::std::cout << "Connecting..." << ::std::endl;
    this->m_communicator.listen(port);
}

PREP_NODISCARD
bool
server::
correct_passwd(const ::std::string& passwd, const data_type& data)
{
    return passwd == data.passwd;
}

void
server::
on_disconnect(tmsocket_t fd)
{
    {
        ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
        this->m_fd_to_id.erase(fd);
    }
    cout << "A client: " << fd << " disconnected!" << endl;
}

void
server::
on_receive(tmsocket_t fd, const ::std::string& msg)
{
    struct invalid_message : ::std::exception
    {
        virtual const char*
        what() const noexcept override
        {
            return "Invalid message.";
        }
    };

    try
    {
        using msg_type = common::protocol::msg_type;
        ::std::istringstream sin;
        sin.str(msg);
        ::std::underlying_type<msg_type>::type msg_type_val;
        sin >> msg_type_val;
        while (sin && sin.get() != '\n');
        if (!sin)
        {
            throw invalid_message();
        }
        switch (static_cast<msg_type>(msg_type_val))
        {
        case msg_type::register_account:
        {
            ::std::string passwd, name;
            ::std::getline(sin, passwd);
            name = msg.substr(sin.tellg());
            if (!sin) throw invalid_message();
            try
            {
                auto id = this->m_database_ptr->add_data(data_type(passwd, name));
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
                    this->m_fd_to_id[fd] = id;
                }
                this->m_communicator.send_to_one_client(fd,
                    ::std::to_string(common::protocol::msg_type_to_integer(msg_type::register_success)) + '\n' +
                    ::std::to_string(id) + '\n');
            }
            catch(...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::register_failed)) + '\n');
            }
            break;
        }
        case msg_type::login:
        {
            id_type id;
            sin >> id;
            while (sin.get() != '\n');
            ::std::string passwd = msg.substr(sin.tellg());
            try
            {
                data_type data;
                if (this->m_database_ptr->get_data(id, data))
                {
                    if (correct_passwd(passwd, data))
                    {
                        {
                            ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
                            if (this->m_fd_to_id.find(fd) == this->m_fd_to_id.end())
                            {
                                this->m_fd_to_id[fd] = id;
                                this->m_communicator.send_to_one_client(fd,
                                    ::std::to_string(common::protocol::msg_type_to_integer(msg_type::login_seccess)) + '\n' +
                                    data.name);
                            }
                            else
                            {
                                // Already login.
                                this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::login_failed)) + '\n');
                            }
                        }
                    }
                    else
                    {
                        // Password incorrect
                        this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::login_failed)) + '\n');
                    }
                }
                else
                {
                    // ID not exist
                    this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::login_failed)) + '\n');
                }
            }
            catch (...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::login_failed)) + '\n');
            }
            break;
        }
        case msg_type::chat:
        {
            try
            {
                auto msg_to_sent = msg.substr(sin.tellg());
                data_type tmp_data;
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
                    auto itr = this->m_fd_to_id.find(fd);
                    if (itr == this->m_fd_to_id.end()) throw invalid_message();
                    for (auto&& fd_to_id : this->m_fd_to_id)
                    {
                        if (fd_to_id.first != fd
                         && this->m_database_ptr->get_data(itr->second, tmp_data))
                        {
                            this->m_communicator.send_to_one_client(fd_to_id.first,
                                ::std::to_string(common::protocol::msg_type_to_integer(msg_type::chat)) + '\n' +
                                tmp_data.name + '\n' + msg_to_sent);
                        }
                            
                    }
                }
                this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::chat_seccess)) + '\n' + msg_to_sent);
            }
            catch (...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(common::protocol::msg_type_to_integer(msg_type::chat_failed)) + '\n' + msg.substr(sin.tellg()));
            }
            break;
        }
        default:
            throw invalid_message();
        }
    }
    catch(const invalid_message&)
    {
        std::cerr << "Server received an invalid message!" << '\n';
    }
}

TMCHAT_NAMESPACE_END
