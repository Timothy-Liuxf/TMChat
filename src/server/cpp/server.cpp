#include "../include/server.hpp"

#include <iostream>
#include <sstream>
#include <protocol/protocol.hpp>
#include <type_traits>

using ::std::cout;
using ::std::cerr;
using ::std::cin;
using ::std::endl;

TMCHAT_NAMESPACE_BEGIN

server::server(create_database_t create_database) : m_database_ptr(create_database())
{
    this->m_communicator.on_listen
    (
        []
        {
            cout << "Successfully connected!" << endl;
        }
    );
    this->m_communicator.add_log([] (const ::std::string& str) { cout << str << endl; });

    this->m_communicator.on_connect([] (int fd) { cout << "A client: " << fd << " connected!" << endl; });
    this->m_communicator.on_reveive([this] (int fd, const ::std::string& msg) { this->on_receive(fd, msg); });

    cout << "Please input server port: " << std::flush;
    ::std::string port;
    ::std::getline(cin, port);

    this->m_communicator.listen(port);
}

PREP_NODISCARD
bool
server::correct_passwd(const ::std::string& passwd, const data_type& data)
{
    return passwd == data.passwd;
}

void
server::on_receive(int fd, const ::std::string& msg)
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
        ::std::istringstream sin;
        sin.str(msg);
        int msg_type_val;
        sin >> msg_type_val;
        while (sin && sin.get() != '\n');
        if (!sin)
        {
            throw invalid_message();
        }
        using protocol::msg_type;
        using protocol::msg_type_to_integer;
        switch (static_cast<msg_type>(msg_type_val))
        {
        case msg_type::register_account:
        {
            ::std::string passwd, name;
            ::std::getline(sin, passwd);
            ::std::getline(sin, name);
            if (!sin) throw invalid_message();
            try
            {
                auto id = this->m_database_ptr->add_data(data_type(passwd, name));
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
                    this->m_fd_to_id[fd] = id;
                }
                this->m_communicator.send_to_one_client(fd,
                    ::std::to_string(msg_type_to_integer(msg_type::register_success)) + '\n' +
                    ::std::to_string(id) + '\n');
            }
            catch(...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::register_failed)) + '\n');
            }
            break;
        }
        case msg_type::login:
        {
            id_type id;
            sin >> id;
            while (sin.get() != '\n');
            ::std::string passwd;
            ::std::getline(sin, passwd);
            if (!sin) throw invalid_message();
            try
            {
                data_type data;
                if (this->m_database_ptr->get_data(id, data))
                {
                    if (correct_passwd(passwd, data))
                    {
                        {
                            ::std::unique_lock<::std::mutex> lock(this->m_fd_to_id_mtx);
                            this->m_fd_to_id[fd] = id;
                        }
                        this->m_communicator.send_to_one_client(fd, 
                            ::std::to_string(msg_type_to_integer(msg_type::login_failed)) + '\n' +
                            data.name + '\n');
                    }
                    else
                    {
                        this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::login_failed)) + '\n');
                    }
                }
                else
                {
                    this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::login_failed)) + '\n');
                }
            }
            catch (...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::login_failed)) + '\n');
            }
            break;
        }
        case msg_type::chat:
        {
            try
            {
                auto msg_to_sent = msg.substr(sin.tellg());
                this->m_communicator.send_to_all_clients(msg_to_sent);
                this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::chat_seccess)) + '\n' + msg_to_sent);
            }
            catch (...)
            {
                this->m_communicator.send_to_one_client(fd, ::std::to_string(msg_type_to_integer(msg_type::chat_failed)) + '\n' + msg.substr(sin.tellg()));
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
