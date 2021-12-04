#include "../include/netexcept.hpp"

#include <malloc.h>
#include <string.h>
#include <string>

TMSOCKET_NAMESPACE_BEGIN

netexcept::netexcept(const char* msg, ::prep::errno_type no)
    : m_no(no), m_str((char*)nullptr, &::free)
{
    ::std::string generate_result = ::std::string(msg) + " Error code: " + ::std::to_string(no);
    char* result = (char*)malloc((generate_result.size() + 1) * sizeof(char));
    ::strcpy(result, generate_result.c_str());
    m_str.reset(result);
}

const char*
netexcept::what() const noexcept
{
    return m_str.get();
}

TMSOCKET_NAMESPACE_END
