#ifndef TMCHAT_SERVER_HPP__
#define TMCHAT_SERVER_HPP__

#include <tmsocket/include/server_communicator.hpp>
#include <memory>
#include <functional>
#include <map>
#include <mutex>

#include "tmchat.h"
#include "i_database.hpp"

TMCHAT_NAMESPACE_BEGIN

struct user_data_t
{
    ::std::string passwd;
    ::std::string name;
    user_data_t() = default;
    user_data_t(const ::std::string& passwd, const ::std::string& name)
                : passwd(passwd), name(name) {}
};

class server
{
public:
    using id_type = typename i_database<user_data_t>::id_type;
    using data_type = user_data_t;
    using create_database_t = ::std::function<::std::unique_ptr<i_database<data_type>>()>;

    server(create_database_t create_database);

private:

    void
    on_receive(int fd, const ::std::string& msg);

    PREP_NODISCARD static
    bool
    correct_passwd(const ::std::string& passwd, const data_type& data);

    ::tmsocket::server_communicator m_communicator;
    ::std::unique_ptr<i_database<data_type>> m_database_ptr;
    ::std::map<int, id_type> m_fd_to_id;
    mutable ::std::mutex m_fd_to_id_mtx;
};

TMCHAT_NAMESPACE_END

#endif // #ifndef TMCHAT_SERVER_HPP__
