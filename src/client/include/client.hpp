#ifndef TMCHAT_CLIENT_HPP__
#define TMCHAT_CLIENT_HPP__

#include <tmsocket/include/client_communicator.hpp>
#include <prep/include/semaphore.hpp>
#include <atomic>
#include <string>

#include "../../common/tmchat.h"

TMCHAT_NAMESPACE_BEGIN

class client
{
public:
    client();

private:
    ::tmsocket::client_communicator m_communicator;
    ::std::atomic_bool m_login_success { false };
    ::prep::concurrent::semaphore m_login_sem { 0, 1 };
    ::std::string m_name;
    ::tmchat::default_id_type m_id;

    void
    on_reveive(const ::std::string& msg);
};

TMCHAT_NAMESPACE_END

#endif // #ifndef TMCHAT_CLIENT_HPP__
