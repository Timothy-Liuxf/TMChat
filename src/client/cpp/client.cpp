#include "../include/client.hpp"
#include "../../common/tmchat.h"
#include "../../common/protocol.hpp"

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <limits>
#include <sstream>

using ::std::cin;
using ::std::cout;
using ::std::cerr;
using ::std::endl;

TMCHAT_NAMESPACE_BEGIN

client::client()
{
    using msg_type = common::protocol::msg_type;

    cout << "Please input server ip (127.0.0.1 by default): " << ::std::flush;
    ::std::string host;
    ::std::getline(std::cin, host);
    if (host == "") host = "127.0.0.1";
    cout << "Please input server port: " << std::flush;
    ::std::string port;
    ::std::getline(std::cin, port);

    bool has_connected = false;
    ::std::mutex has_connected_mtx;
    ::std::condition_variable has_connected_cond;
    ::std::atomic_bool connection_failed { false };

    this->m_communicator.on_connect
    (
        [&]
        {
            {
                ::std::unique_lock<::std::mutex> lock(has_connected_mtx);
                has_connected = true;
                has_connected_cond.notify_all();
            }
            ::std::cout << "Successfully connect to server!" << ::std::endl;
        }
    );
    this->m_communicator.add_log([](const ::std::string& str) { cout << str << endl; });
    this->m_communicator.on_reveive([this](const ::std::string& msg) { this->on_reveive(msg); });

    ::std::thread thr
    (
        [&, this]
        {
            {
                ::std::unique_lock<::std::mutex> lock(has_connected_mtx);
                has_connected_cond.wait(lock, [&] { return has_connected; });
            }

            if (connection_failed)
            {
                return;
            }

            cout << "Please choose:\nA. login B. register C. quit\n" << ::std::flush;
            char ch = cin.get();
            while (cin)
            {
                while (cin.get() != '\n');
                switch (ch)
                {
                default:
                    cout << "Illegal! Please input again!" << endl;
                    ch = cin.get();
                    continue;
                break;
                case 'B':
                {
                    ::std::string name, passwd;
                    cout << "Please input your username: " << endl;
                    ::std::getline(cin, name);
                    cout << "Please input your password: " << endl;
                    ::std::getline(cin, passwd);
                    if (!cin)
                    {
                        cout << "Input illegal!" << endl;
                        cin.clear();
                        cin.ignore((::std::numeric_limits<::std::streamsize>::max)(), '\n');
                        break;
                    }
                    this->m_communicator.send_to_server(::std::to_string(common::protocol::msg_type_to_integer(msg_type::register_account)) + '\n' +
                                                        passwd + '\n' + name);
                    this->m_login_sem.acquire();
                    if (this->m_login_success)
                    {
                        cout << "Register success!" << endl;
                        this->m_name = name;
                    }
                    else
                    {
                        cout << "Register failed!" << endl;
                    }
                    break;
                }
                case 'A':
                {
                    tmchat::default_id_type id;
                    ::std::string passwd;
                    cout << "Please input your id: " << endl;
                    cin >> id;
                    while (cin && cin.get() != '\n');
                    cout << "Please input your password: " << endl;
                    ::std::getline(cin, passwd);
                    if (!cin)
                    {
                        cout << "Input illegal!" << endl;
                        cin.clear();
                        cin.ignore((::std::numeric_limits<::std::streamsize>::max)(), '\n');
                        break;
                    }
                    this->m_communicator.send_to_server(::std::to_string(common::protocol::msg_type_to_integer(msg_type::login)) + '\n' +
                                                        ::std::to_string(id) + '\n' + passwd);
                    this->m_login_sem.acquire();
                    if (this->m_login_success)
                    {
                        cout << "Login success!" << endl;
                        this->m_id = id;
                    }
                    else
                    {
                        cout << "Login failed!" << endl;
                    }
                    break;
                }
                case 'C':
                {
                    break;
                }
                }
                break;
            }

            if (this->m_login_success)
            {
                cout << endl << "\n====================\n";
                cout << "Your ID: " << this->m_id
                     << "\n--------------------\nYour username: " << this->m_name
                     << "\n====================\n" << endl;
                cout << "Start chatting now!\n" << endl;
                ::std::string buf;
                while (true)
                {
                    ::std::getline(cin, buf);
                    if (!cin)
                    {
                        cin.clear();
                        cin.ignore((::std::numeric_limits<::std::streamsize>::max)(), '\n');
                        break;
                    }
                    this->m_communicator.send_to_server(::std::to_string(common::protocol::msg_type_to_integer(msg_type::chat)) + '\n' + buf);
                }
            }

            this->m_communicator.end_communication();
        }
    );

    try
    {
        ::std::cout << "Connecting..." << ::std::endl;
        this->m_communicator.connect(host, port);
        thr.join();
    }
    catch (...)
    {
        has_connected = true;
        connection_failed = true;
        has_connected_cond.notify_all();
        thr.join();
        throw;
    }
}

void
client::on_reveive(const ::std::string& msg)
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
        case msg_type::register_failed:
        case msg_type::login_failed:
        {
            this->m_login_sem.release();
            break;
        }
        case msg_type::register_success:
        {
            sin >> this->m_id;
            if (sin)
            {
                this->m_login_success = true;
            }
            this->m_login_sem.release();
            break;
        }
        case msg_type::login_seccess:
        {
            this->m_name = msg.substr(sin.tellg());
            this->m_login_success = true;
            this->m_login_sem.release();
            break;
        }
        case msg_type::chat:
        {
            ::std::string from_name;
            ::std::getline(sin, from_name);
            if (!sin) throw invalid_message();
            auto msg_recv = msg.substr(sin.tellg());
            cout << "[" << from_name << "]: " << msg_recv << endl;
            break;
        }
        case msg_type::chat_seccess:
        {
            cout << "(You)[" << this->m_name << "]: " << msg.substr(sin.tellg()) << endl;
            break;
        }
        case msg_type::chat_failed:
        {
            cout << "[!]: Fail to send message: " << msg.substr(sin.tellg()) << endl;
            break;
        }
        default:
            throw invalid_message();
        }
    }
    catch (const invalid_message&)
    {
        cerr << "Received an invalid message!" << '\n';
    }
    catch (::std::exception& e)
    {
        cerr << e.what() << '\n';
    }
}

TMCHAT_NAMESPACE_END
