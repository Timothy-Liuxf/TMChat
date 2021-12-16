#ifndef TMCHAT_LOCAL_DATABASE_HPP__
#define TMCHAT_LOCAL_DATABASE_HPP__

#include "i_database.hpp"
#include "tmchat.h"
#include <string>
#include <map>
#include <mutex>
#include <limits>

#include "server.hpp"

TMCHAT_NAMESPACE_BEGIN

class local_database : public i_database<user_data_t>
{
public:

    static_assert(sizeof(id_type) <= sizeof(::std::size_t), "The maximum number of users must be no more than the maximum of ::std::size_t!");

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
        if (this->m_database.size() == ::std::numeric_limits<id_type>::max())
            throw database_full();
        id_type cnt = 0;
        for (auto&& data_tip : this->m_database)
        {
            if (data_tip.first != cnt)
            {
                this->m_database.emplace(cnt, data);
                return cnt;
            }
            ++cnt;
        }
        throw database_full();
    }

    PREP_NODISCARD
    virtual bool
    id_exists(const id_type& id) const override
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        return this->m_database.find(id) != this->m_database.end();
    }

    PREP_NODISCARD
    virtual bool
    remove_data(const id_type& id) override
    {
        ::std::unique_lock<::std::mutex> lock(this->m_database_mtx);
        return this->m_database.erase(id) == 1;
    }

    PREP_NODISCARD
    virtual bool
    get_data(const id_type& id, data_type& out_data) const override
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
