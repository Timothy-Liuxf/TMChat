#ifndef PREP_CONCURRENT_QUEUE_HPP__
#define PREP_CONCURRENT_QUEUE_HPP__

#include <prep/include/prep.h>
#include <prep/include/namespace.hpp>

#include <queue>
#include <mutex>
#include <utility>
#include <condition_variable>

PREP_CONCURRENT_NAMESPACE_BEGIN

template <typename Elem, typename Container = ::std::deque<Elem>>
class concurrent_queue
{
private:
    using queue_type        = ::std::queue<Elem, Container>;
    using lock_type         = ::std::unique_lock<::std::mutex>;

public:
    using size_type         = typename queue_type::size_type;
    using value_type        = typename queue_type::value_type;
    using reference         = typename queue_type::reference;
    using const_reference   = typename queue_type::const_reference;
    using container_type    = typename queue_type::container_type;

    concurrent_queue() = default;
    concurrent_queue(const concurrent_queue&) = delete;
    ~concurrent_queue() = default;
    concurrent_queue&
    operator=(const concurrent_queue&) = delete;

    void
    clear()
    {
        lock_type lock(this->m_mtx);
        while (!this->m_q.empty()) this->m_q.pop();
    }

    PREP_NODISCARD
    size_type
    size() const
    {
        lock_type lock(this->m_mtx);
        return this->m_q.size();
    }

    PREP_NODISCARD
    bool
    empty() const
    {
        return this->size() == 0;
    }

    template <typename... Ts>
    void
    emplace(Ts&&... args)
    {
        lock_type lock(this->m_mtx);
        this->m_q.emplace(::std::forward<Ts>(args)...);
        this->m_cond.notify_one();
    }

    void
    push(const value_type& e)
    {
        this->emplace(e);
    }

    void
    push(value_type&& e)
    {
        this->emplace(::std::move(e));
    }

    PREP_NODISCARD
    bool
    try_pop(value_type& out)
    {
        lock_type lock(this->m_mtx);
        if (this->size() == 0) return false;
        out = ::std::move(this->m_q.pop());
        this->m_q.pop();
        return true;
    }

    PREP_NODISCARD
    value_type
    wait_for_pop()
    {
        lock_type lock(this->m_mtx);
        this->m_cond.wait(lock, [this] { return this->m_q.size() > 0; });
        value_type out = ::std::move(this->m_q.front());
        m_q.pop();
        return out;
    }

private:
    queue_type m_q;
    mutable ::std::mutex m_mtx;
    ::std::condition_variable m_cond;
};

PREP_CONCURRENT_NAMESPACE_END

#endif // #ifndef PREP_CONCURRENT_QUEUE_HPP__
