#ifndef PREP_CONCURRENT_LIST_HPP__
#define PREP_CONCURRENT_LIST_HPP__

#include <prep/include/prep.h>
#include <prep/include/namespace.hpp>

#include <list>
#include <utility>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

PREP_CONCURRENT_NAMESPACE_BEGIN

template <typename Elem, typename Alloc = ::std::allocator<Elem>>
class concurrent_list
{
private:
    using list_type         = ::std::list<Elem, Alloc>;
    using lock_type         = ::std::unique_lock<::std::mutex>;

public:
    using size_type         = typename list_type::size_type;
    using value_type        = typename list_type::value_type;
    using reference         = typename list_type::reference;
    using const_reference   = typename list_type::const_reference;

    concurrent_list() = default;
    concurrent_list(const concurrent_list&) = delete;
    ~concurrent_list() = default;
    concurrent_list&
    operator=(const concurrent_list&) = delete;

    void clear()
    {
        lock_type lock(this->m_mtx);
        this->m_list.clear();
    }

    PREP_NODISCARD
    size_type
    size() const
    {
        lock_type lock(this->m_mtx);
        return this->m_list.size();
    }

    PREP_NODISCARD
    bool
    empty() const
    {
        return this->size() == 0;
    }

    template <typename... Ts>
    void
    emplace_back(Ts&&... args)
    {
        lock_type lock(this->m_mtx);
        this->m_list.emplace_back(::std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void
    emplace_front(Ts&&... args)
    {
        lock_type lock(this->m_mtx);
        this->m_list.emplace_front(::std::forward<Ts>(args)...);
    }

    void
    push_back(const value_type& e)
    {
        this->emplace_back(e);
    }

    void
    push_back(value_type&& e)
    {
        this->emplace_back(::std::move(e));
    }

    void
    push_front(const value_type& e)
    {
        this->emplace_front(e);
    }

    void
    push_front(value_type&& e)
    {
        this->emplace_front(::std::move(e));
    }

    template <typename Ret, typename ElemConv>
    void
    visit(::std::function<Ret(ElemConv)> func)
    {
        lock_type lock(this->m_mtx);
        for (auto& elem : this->m_list)
        {
            (void)func(elem);
        }
    }

private:
    list_type m_list;
    mutable ::std::mutex m_mtx;
    ::std::condition_variable m_cond;
};

PREP_CONCURRENT_NAMESPACE_END

#endif // #ifndef PREP_CONCURRENT_LIST_HPP__
