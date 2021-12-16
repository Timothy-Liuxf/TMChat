#if !defined(TMSOCKET_SERVER_COMMUNICATOR_HPP__) && !defined(TMSOCKET_CLIENT_COMMUNICATOR_HPP__)
#   error Please include <tmsocket/include/server_communicator.hpp> or <tmsocket/include/client_communicator.hpp> instead.
#endif

#include <prep/include/prep.h>
#include <cstddef>
#include <string>
#include <utility>
#include <algorithm>

TMSOCKET_NAMESPACE_BEGIN

#define PROTOCAL_INLINE_NAMESPACE_BEGIN inline namespace protocol {
#define PROTOCAL_INLINE_NAMESPACE_END }

PROTOCAL_INLINE_NAMESPACE_BEGIN

#define TMSOCKET_PREFIX_STR     "tmsocket_prefix"
#define TMSOCKET_POSTFIX_STR    "tmsocket_postfix"
#define TMSOCKET_ENDMSG_STR     "tmsocket_endmsg"

static const ::std::string tmsocket_prefix  { TMSOCKET_PREFIX_STR };
static const ::std::string tmsocket_postfix { TMSOCKET_POSTFIX_STR };
static const ::std::string tmsocket_endmsg  { TMSOCKET_ENDMSG_STR };

#undef TMSOCKET_ENDMSG_STR
#undef TMSOCKET_POSTFIX_STR
#undef TMSOCKET_PREFIX_STR

class protocol
{
public:
    PREP_NODISCARD
    static bool
    try_decode_message(::std::string& buffer, ::std::string& msg)
    {
        const auto prefix_pos = buffer.find(tmsocket_prefix);
        if (prefix_pos == ::std::string::npos)
        {
            return false;
        }

        ::std::string org_buffer = buffer;
        auto postfix_pos = buffer.find(tmsocket_postfix, prefix_pos + tmsocket_prefix.size());
        while (postfix_pos != ::std::string::npos)
        {
            if (buffer.substr(postfix_pos + tmsocket_postfix.size(), tmsocket_postfix.size()) == tmsocket_postfix)
            {
                buffer.erase(postfix_pos + tmsocket_postfix.size(), tmsocket_postfix.size());
            }
            else if (buffer.substr(postfix_pos + tmsocket_postfix.size(), tmsocket_endmsg.size()) == tmsocket_endmsg)
            {
                msg = buffer.substr(prefix_pos + tmsocket_prefix.size(), postfix_pos - (prefix_pos + tmsocket_prefix.size()));
                buffer = buffer.substr(postfix_pos + tmsocket_postfix.size() + tmsocket_endmsg.size());
                return true;
            }
            postfix_pos = buffer.find(tmsocket_postfix, postfix_pos + tmsocket_postfix.size());
        }
    
        buffer = ::std::move(org_buffer);
        if (prefix_pos != 0)
        {
            buffer = buffer.substr(prefix_pos);
        }
        return false;
    }

    PREP_NODISCARD
    static ::std::string
    encode_message(const ::std::string& origin_msg)
    {
        ::std::string ret { tmsocket_prefix };
        auto postfix_pos = origin_msg.find(tmsocket_postfix);
        auto copied_itr = origin_msg.begin();
        while (postfix_pos != ::std::string::npos)
        {
            ret.append(copied_itr, origin_msg.begin() + postfix_pos);
            copied_itr = origin_msg.begin() + postfix_pos;
            ret.append(tmsocket_postfix);
            postfix_pos = origin_msg.find(tmsocket_postfix, postfix_pos + tmsocket_postfix.size());
        }
        ret.append(copied_itr, origin_msg.end());
        ret.append(tmsocket_postfix);
        ret.append(tmsocket_endmsg);
        return ret;
    }
};

PROTOCAL_INLINE_NAMESPACE_END

#undef PROTOCAL_INLINE_NAMESPACE_END
#undef PROTOCAL_INLINE_NAMESPACE_BEGIN

TMSOCKET_NAMESPACE_END
