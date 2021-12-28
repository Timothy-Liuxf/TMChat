#include "../include/server_stream.hpp"

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

server_stream::
~server_stream() noexcept
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
server_stream::
pick_msg()
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
                this->m_logger.invoke(msg.second.second);
                break;
            case msg_type::critical_error:
                throw ::std::runtime_error(::std::string(::std::move(msg.second.second)));
                break;
            case msg_type::msg:
                this->m_on_receive.invoke(msg.second.first, msg.second.second);
                break;
            case msg_type::disconnect:
                this->m_on_disconnect.invoke(msg.second.first);
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
server_stream::
wait_for_finish_pick() const
{
    ::std::unique_lock<::std::mutex> lock(this->m_finish_pick_mtx);
    this->m_finish_pick_cond.wait(lock, [this] { return this->m_finish_pick; });
}


void
server_stream::
end_communication()
{
    bool org_val = m_is_finished.exchange(true);
    if (!org_val)
    {
        this->m_msg_q.emplace(msg_type::finish, ::std::make_pair<int64_t, ::std::string>(0, ""));
        if (this->m_begin_pick_msg) this->wait_for_finish_pick();
        if (!this->m_is_connected) return;
        this->m_accept_clients_sem->acquire();
        this->m_accept_clients_sem.reset();
        this->m_receive_from_client_sems.visit<void, ::std::shared_ptr<prep::concurrent::semaphore>&>
        (
            [] (::std::shared_ptr<prep::concurrent::semaphore>& sem) noexcept
            {
                try
                {
                    if (sem)
                    {
                        sem->acquire();
                        sem.reset();
                    }
                }
                catch(...)
                {
                    // No operation
                }
                
            }
        );
        ::close_socket(this->m_fd);
        this->m_fd = -1;
    }
}

void
server_stream::
receive_from_client(tmsocket_t client_fd, ::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept
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

                int len = ::recv(client_fd, buf.data(), this->buf_size(), 0);

                if (!try_operate_sem(true))         // Acquire semaphore
                {
                    // Communication has been closed
                    return;
                }

                if (receive_error(len))
                {
                    if (!this->is_finished())
                        this->m_msg_q.emplace(msg_type::error, ::std::pair<int64_t, ::std::string>(client_fd, "Fail to receive message!"));
                }
                else if (socket_shutdown(len))
                {
                    {
                        ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
                        this->m_client_fds.erase(client_fd);
                    }
                    this->m_msg_q.emplace(msg_type::disconnect, ::std::pair<int64_t, ::std::string>(client_fd, "A client closed the connection!"));
                    break;
                }
                else
                {
                    this->m_msg_q.emplace(msg_type::msg, ::std::pair<int64_t, ::std::string>(client_fd, ::std::string(buf.data(), len)));
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
        this->m_msg_q.emplace(msg_type::critical_error, ::std::pair<int64_t, ::std::string>(0, "Cannot allocate buffer!"));
    }
    catch (::std::exception& e)
    {
        this->m_msg_q.emplace(msg_type::error, ::std::pair<int64_t, ::std::string>(0, e.what()));
    }
}

void
server_stream::
accept_clients(::std::weak_ptr<::prep::concurrent::semaphore> sem_ptr) noexcept
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
            while (!this->is_finished())
            {
                /*sockaddr clientaddr;
                socklen_t clientaddr_len = sizeof(clientaddr);
                ::std::memset(&clientaddr, 0, sizeof(clientaddr));*/
                auto fd = this->m_fd;

                if (!try_operate_sem(false))        // Release semaphore
                {
                    // Here shouldn't approach
                    assert(0);
                }

                tmsocket_t client_fd = ::accept(fd, nullptr, nullptr);

                if (!try_operate_sem(true))         // Acquire semaphore
                {
                    // Communication has been closed
                    return;
                }
                
                if (invalid_socket(client_fd))
                {
                    if (!this->is_finished()) this->m_msg_q.emplace(msg_type::error,
                    ::std::pair<int64_t, ::std::string>(static_cast<int64_t>(get_net_error()), netexcept{"Accept client failed!", get_net_error()}.what())
                    );
                }
                else
                {
                    ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
                    this->m_client_fds.insert(client_fd);
                    this->m_on_connect.invoke(client_fd);

                    // Must first wait accept_clients to end in function 'end_communication',
                    // or this->receive_from_client_sems may be cleared before sem_ptr is pushed,
                    // this will cause the new thread will still be activate after the communication ends.
                    auto sem_ptr = ::std::make_shared<prep::concurrent::semaphore>(1, 1); 
                    this->m_receive_from_client_sems.emplace_back(sem_ptr);
                    ::std::thread { &server_stream::receive_from_client, this, client_fd, sem_ptr }.detach();
                }
            }
            if (!try_operate_sem(false))            // Release semaphore
            {
                // Here shouldn't approach
                assert(0);
            }
        }
        catch(...)
        {
            try_operate_sem(false);     // Release semaphore
            throw;
        }
        
    }
    catch (::std::exception& e)
    {
        this->m_msg_q.emplace(msg_type::critical_error, ::std::pair<int64_t, ::std::string>(0, e.what()));
    }
}

void
server_stream::
listen(const ::std::string& host, const ::std::string& port)
{
    {
        ::std::unique_lock<::std::mutex> lock(this->m_connect_mtx);
        
        this->m_fd = connect_impl(host, port, this, &::bind);
        this->m_is_connected = true;

        try
        {
            int ret;
            ret = ::listen(this->m_fd, 1024);
            if (ret != 0)
            {
                throw fail_to_listen("Fail to listen!", get_net_error());
            }
        }
        catch(...)
        {
            ::close_socket(this->m_fd);
            this->m_fd = -1;
            throw;
        }
    }
    
    this->m_on_listen.invoke();
    this->m_accept_clients_sem = ::std::make_shared<prep::concurrent::semaphore>(1, 1);
    ::std::thread { &server_stream::accept_clients, this, this->m_accept_clients_sem }.detach();

    this->m_begin_pick_msg = true;
    this->pick_msg();
}

void
server_stream::
send_to_one_client(tmsocket_t client_fd, const ::std::string& msg)
{
    if (!this->is_connected())
    {
        throw ::std::logic_error("Hasn't connected!");
    }
    if (this->is_finished())
    {
        throw ::std::logic_error("Communication has finished!");
    }
    
    {
        ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
        if (this->m_client_fds.find(client_fd) == this->m_client_fds.end())
        {
            throw client_not_exist(client_fd);
        }
        send_msg(client_fd, msg, this);
    }
}

void
server_stream::
send_to_all_clients(const ::std::string& msg)
{
    ::std::unique_lock<::std::mutex> lock(this->m_client_fds_lock);
    for (auto client_fd : this->m_client_fds)
    {
        try
        {
            send_msg(client_fd, msg, this);
        }
        catch (...)
        {
            // No operation
        }
    }
}

TMSOCKET_NAMESPACE_END
