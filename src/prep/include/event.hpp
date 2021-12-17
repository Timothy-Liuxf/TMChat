#ifndef PREP_EVENT_HPP
#define PREP_EVENT_HPP

#include <prep/include/prep.h>
#include <prep/include/namespace.hpp>
#include <mutex>
#include <vector>
#include <functional>
#include <utility>

PREP_CONCURRENT_NAMESPACE_BEGIN

template <typename... Args>
class event
{
private:
    using func_type = ::std::function<void(Args...)>;

public:
    event() = default;
    event(const event&) = delete;
    ~event() = default;
    event& operator=(const event&) = delete;

    void
    invoke(Args... args) const
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        for (auto&& manner : this->m_manners)
        {
            manner(::std::forward<Args>(args)...);
        }
    }

    void
    subscript(const func_type& func)
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        m_manners.emplace_back(func);
    }

    void
    subscript(func_type&& func)
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        m_manners.emplace_back(::std::move(func));
    }

private:
    ::std::vector<func_type> m_manners;
    mutable ::std::mutex m_mtx;
};

PREP_CONCURRENT_NAMESPACE_END

#endif
