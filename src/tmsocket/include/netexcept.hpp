#ifndef TMSOCKET_NETEXCEPT_HPP__
#define TMSOCKET_NETEXCEPT_HPP__

#include <prep/include/prep.h>
#include <prep/include/os.h>
#include <tmsocket/include/defs.hpp>
#include <exception>
#include <memory>

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

class invalid_input : public netexcept
{
public:
    invalid_input(const char* msg, prep::errno_type no) : netexcept(msg, no) {}
};

TMSOCKET_NAMESPACE_END

#endif
