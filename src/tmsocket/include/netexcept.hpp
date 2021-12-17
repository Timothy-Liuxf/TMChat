#ifndef TMSOCKET_NETEXCEPT_HPP__
#define TMSOCKET_NETEXCEPT_HPP__

#include <prep/include/prep.h>
#include <prep/include/os.h>
#include <prep/include/os_net_defs.h>
#include <tmsocket/include/defs.hpp>
#include <exception>
#include <memory>
#include <string>

TMSOCKET_NAMESPACE_BEGIN

class tmsocket_exception : ::std::exception {};

class netexcept : public tmsocket_exception
{
public:

    PREP_NODISCARD
    virtual const char*
    what() const noexcept override;

    netexcept(const char* msg, prep::errno_type no);
    prep::errno_type
    get_errno() const
    { return this->m_no; }

private:
    ::prep::errno_type m_no;
    ::std::unique_ptr<char, void(*)(void*)> m_str;
};


class initialize_network_failed : public netexcept
{
public:
    initialize_network_failed(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class invalid_input : public netexcept
{
public:
    invalid_input(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_init_socket : public netexcept
{
public:
    fail_to_init_socket(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_connect : public netexcept
{
public:
    fail_to_connect(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_listen : public netexcept
{
public:
    fail_to_listen(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

class fail_to_send : public netexcept
{
public:
    fail_to_send(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};


class client_not_exist : public tmsocket_exception
{
public:
    client_not_exist(tmsocket_t fd) : m_fd(fd) {}

    PREP_NODISCARD
    virtual const char*
    what() const noexcept override
    {
        return "The client specified doesn't exists!";
    }

    PREP_NODISCARD
    tmsocket_t
    get_fd() const noexcept
    {
        return this->m_fd;
    }

private:
    tmsocket_t m_fd;
};

class disconnect_unexpectedly : public tmsocket_exception
{
public:
    explicit disconnect_unexpectedly(const ::std::string& str) : m_str(str) {}
    explicit disconnect_unexpectedly(::std::string&& str) noexcept : m_str(::std::move(str)) {}
    disconnect_unexpectedly(const disconnect_unexpectedly& ex) : m_str(ex.m_str) {}
    disconnect_unexpectedly(disconnect_unexpectedly&& ex) noexcept : m_str(::std::move(ex.m_str)) {}
    disconnect_unexpectedly& operator=(const disconnect_unexpectedly& ex)
    {
        this->m_str = ex.m_str;
        return *this;
    }
    disconnect_unexpectedly& operator=(disconnect_unexpectedly&& ex) noexcept
    {
        this->m_str = ::std::move(ex.m_str);
        return *this;
    }

    PREP_NODISCARD
    virtual const char*
    what() const noexcept override
    {
        return m_str.c_str();
    }

private:
    ::std::string m_str;
};

TMSOCKET_NAMESPACE_END

#endif
