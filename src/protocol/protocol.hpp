#ifndef TMCHAT_PROTOCOL_PROTOCOL_HPP__
#define TMCHAT_PROTOCOL_PROTOCOL_HPP__

#define TMCHAT_PRTOCOL_NAMESPACE_BEGIN namespace tmchat { namespace protocol {
#define TMCHAT_PRTOCOL_NAMESPACE_END } }

#include <prep/include/prep.h>
#include <cstdint>
#include <type_traits>

TMCHAT_PRTOCOL_NAMESPACE_BEGIN

enum class msg_type : uint32_t
{
    none = 0,
    register_account = 1,   // type \n passwd \n username \n
    register_success = 2,   // type \n id \n
    register_failed = 3,    // type \n
    login = 4,              // type \n id \n passwd \n
    login_seccess = 5,      // type \n name \n
    login_failed = 6,       // type \n
    chat = 10,              // type \n message
    chat_seccess = 11,      // type \n message
    chat_failed = 12        // type \n message
};

PREP_NODISCARD PREP_FORCED_INLINE
static typename ::std::underlying_type<msg_type>::type
msg_type_to_integer(msg_type val)
{
    return static_cast<typename ::std::underlying_type<msg_type>::type>(val);
}

TMCHAT_PRTOCOL_NAMESPACE_END

#undef TMCHAT_PRTOCOL_NAMESPACE_END
#undef TMCHAT_PRTOCOL_NAMESPACE_BEGIN

#endif // #ifndef TMCHAT_PROTOCOL_PROTOCOL_HPP__
