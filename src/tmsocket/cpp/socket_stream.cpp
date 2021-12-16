#include "../include/socket_stream.hpp"

TMSOCKET_NAMESPACE_BEGIN

void
socket_stream::pick_msg()
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
socket_stream::wait_for_finish_pick() const
{
    ::std::unique_lock<::std::mutex> lock(this->m_finish_pick_mtx);
    this->m_finish_pick_cond.wait(lock, [this] { return this->m_finish_pick; });
}

TMSOCKET_NAMESPACE_END
