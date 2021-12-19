#include "../include/client_stream.hpp"

#include <prep/include/os_net.h>
#include <prep/include/event.hpp>
#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/netexcept.hpp>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <cassert>
#include <vector>
#include <string>
#include <atomic>
#include <functional>
#include <utility>
#include <memory>
#include <mutex>
#include <cstddef>
#include <limits>
#include <condition_variable>

TMSOCKET_NAMESPACE_BEGIN

#include "details/socket_impl.ipp"

client_stream::~client_stream() noexcept
{
    try
    {
        this->end_communication();
    }
    catch(...)
    {
        // No operation
    }
    
}


void
client_stream::pick_msg()
{
    try
    {
        while (true)
        {
            auto msg = this->m_msg_q.wait_for_pop();

            switch (msg.first)
            {
            case msg_type::finish:
                goto quit;
            case msg_type::log:
            case msg_type::error:
                this->m_logger.invoke(msg.second);
                break;
            case msg_type::critical_error:
                throw ::std::runtime_error(::std::string(::std::move(msg.second)));
                break;
            case msg_type::msg:
                this->m_on_receive.invoke(msg.second);
                break;
            case msg_type::disconnect_unexpectly:
                throw disconnect_unexpectedly(::std::move(msg.second));
                break;
            }
        }
    quit:
        {
            ::std::unique_lock<::std::mutex> lock(this->m_finish_pick_mtx);
            this->m_finish_pick = true;
            this->m_finish_pick_cond.notify_all();
        }
    }
    catch (...)
    {
        {
            ::std::unique_lock<::std::mutex> lock(this->m_finish_pick_mtx);
            this->m_finish_pick = true;
            this->m_finish_pick_cond.notify_all();
        }
        throw;
    }
}

void
client_stream::wait_for_finish_pick() const
{
    ::std::unique_lock<::std::mutex> lock(this->m_finish_pick_mtx);
    this->m_finish_pick_cond.wait(lock, [this] { return this->m_finish_pick; });
}


void
client_stream::receive_from_server(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept
{
    try
    {
        ::std::function<bool(bool)> try_operate_sem = [&] (bool try_acquire)
        {
            auto act_ptr = sem_ptr.lock();
            if (!static_cast<bool>(act_ptr))
            {
                return false;
            }
            if (try_acquire) act_ptr->acquire();
            else act_ptr->release();
            return true;
        };

        if (!try_operate_sem(true))     // Acquire semaphore
        {
            return;
        }

        try
        {
            ::std::vector<char> buf(this->buf_size());
            while (!this->is_finished())
            {
                if (!try_operate_sem(false))        // Release semaphore
                {
                    // Here shouldn't approach
                    assert(0);
                }

                int len = ::recv(this->m_fd, buf.data(), static_cast<int>(this->buf_size()), 0);
                
                if (!try_operate_sem(true))         // Acquire semaphore
                {
                    // Communication has been closed
                    return;
                }
                
                if (receive_error(len))
                {
                    if (!this->is_finished()) this->m_msg_q.emplace(msg_type::critical_error, "Fail to receive message!");
                }
                else if (socket_shutdown(len))
                {
                    this->m_msg_q.emplace(msg_type::disconnect_unexpectly, "Server closed the connection.");
                }
                else
                {
                    this->m_msg_q.emplace(msg_type::msg, ::std::string(buf.data(), len));
                }
            }

            if (!try_operate_sem(false))            // Release semaphore
            {
                // Here shouldn't approach
                assert(0);
            }
        }
        catch (...)
        {
            try_operate_sem(false);     // Release semaphore
            throw;
        }
        
    }
    catch (::std::bad_alloc&)
    {
        this->m_msg_q.emplace(msg_type::critical_error, "Cannot allocate buffer!");
    }
    catch (::std::exception& e)
    {
        this->m_msg_q.emplace(msg_type::critical_error, e.what());
    }
}

void
client_stream::connect(const ::std::string& host, const ::std::string& port)
{
    {
        ::std::unique_lock<::std::mutex> lock(this->m_connect_mtx);
        this->m_fd = connect_impl(host, port, this, &::connect);
        this->m_is_connected = true;
    }
    this->m_on_connect.invoke();

    auto sem_ptr = ::std::make_shared<::prep::concurrent::semaphore>(1, 1);
    this->m_receive_from_server_sem = sem_ptr;
    ::std::thread { &client_stream::receive_from_server, this, sem_ptr }.detach();
    this->m_begin_pick_msg = true;
    this->pick_msg();
}

void
client_stream::send_to_server(const ::std::string& msg)
{
    send_msg(this->m_fd, msg, this);
}

void
client_stream::end_communication()
{
    bool org_val = this->m_is_finished.exchange(true);
    if (!org_val)
    {
        this->m_msg_q.emplace(msg_type::finish, "");
        if (this->m_begin_pick_msg) this->wait_for_finish_pick();
        if (!this->m_is_connected) return;
        this->m_receive_from_server_sem->acquire();
        this->m_receive_from_server_sem.reset();
        ::close_socket(this->m_fd);
        this->m_fd = -1;
    }
}

TMSOCKET_NAMESPACE_END
