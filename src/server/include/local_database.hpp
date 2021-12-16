#ifndef TMCHAT_LOCAL_DATABASE_HPP__
#define TMCHAT_LOCAL_DATABASE_HPP__

#include "i_database.hpp"
#include "tmchat.h"
#include <string>
#include <map>
#include <mutex>

TMCHAT_NAMESPACE_BEGIN

struct user_data_t
{
    ::std::string passwd;
    ::std::string name;
};

class local_database : public i_database<user_data_t>
{
public:
    local_database() = default;
    local_database(const local_database&) = delete;
    local_database& operator=(const local_database&) = delete;

    PREP_NODISCARD
    virtual ::std::size_t
    size() const override
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        return this->m_database.size();
    }

    PREP_NODISCARD
    virtual id_type
    add_data(const data_type& data) override
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        id_type cnt = 0;
        for (auto&& data : this->m_database)
        {
            if (data.first != cnt)
            {
                this->m_database.emplace(cnt, data);
                return cnt;
            }
            ++cnt;
        }
    }

    PREP_NODISCARD
    virtual bool
    id_exists(const id_type& id) const
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        return this->m_database.find(id) != this->m_database.end();
    }

    PREP_NODISCARD
    virtual bool
    remove_data(const id_type& id)
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        return this->m_database.erase(id) == 1;
    }

    PREP_NODISCARD
    virtual bool
    get_data(const id_type& id, data_type& out_data) const
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        auto itr = this->m_database.find(id);
        if (itr == this->m_database.end()) return false;
        else
        {
            out_data = itr->second;
            return true;
        }
    }

private:
    ::std::map<id_type, data_type> m_database;
    mutable ::std::mutex m_database_mtx;
};

TMCHAT_NAMESPACE_END

#endif // #ifndef TMCHAT_LOCAL_DATABASE_HPP__
