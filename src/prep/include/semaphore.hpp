#ifndef PREP_SEMAPHORE_HPP__
#define PREP_SEMAPHORE_HPP__

#include <prep/include/compiler.h>
#include <prep/include/namespace.hpp>

#include <mutex>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <stdexcept>

PREP_CONCURRENT_NAMESPACE_BEGIN

class semaphore_overflow : public ::std::exception
{
    virtual const char*
    what() const noexcept
    {
        return "Semaphore overflow!";
    }
};

class semaphore
{
public:
    using size_type = ::std::size_t;

    semaphore(size_type init_val, size_type max_val) : m_cnt(init_val), m_max(max_val)
    {
        if (init_val > max_val)
        {
            throw ::std::invalid_argument("The initial value of the semaphore must be less max the max value!");
        }
    }

    semaphore(const semaphore&) = delete;
    semaphore& operator=(const semaphore&) = delete;
    ~semaphore() = default;

    void
    acquire()
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        this->m_cond.wait(lock, [this] { return this->m_cnt > 0; });
        --this->m_cnt;
    }

    PREP_NODISCARD
    bool
    try_acquire()
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        if (this->m_cnt > 0)
        {
            --this->m_cnt;
            return true;
        }
        else
        {
            return false;
        }
    }

    void
    release()
    {
        ::std::unique_lock<::std::mutex> lock(this->m_mtx);
        if (this->m_cnt == this->m_max)
        {
            throw semaphore_overflow{};
        }
        ++this->m_cnt;
        this->m_cond.notify_one();
    }

private:
    size_type m_cnt;
    const size_type m_max;
    ::std::mutex m_mtx;
    ::std::condition_variable m_cond;
};

PREP_CONCURRENT_NAMESPACE_END

#endif
