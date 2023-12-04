#if !defined(TMSOCKET_SERVER_COMMUNICATOR_HPP__) && !defined(TMSOCKET_CLIENT_COMMUNICATOR_HPP__)
#   error Please include <tmsocket/include/server_communicator.hpp> or <tmsocket/include/client_communicator.hpp> instead.
#endif

#ifndef TMSOCKET_DETAILS_PROTOCOL_IPP__
#define TMSOCKET_DETAILS_PROTOCOL_IPP__

#define PROTOCAL_INLINE_NAMESPACE_BEGIN inline namespace protocol_ns {
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
            auto repeated_postfix_beg
                = buffer.cbegin() + postfix_pos + tmsocket_postfix.size();
            if (static_cast<std::size_t>(buffer.cend() - repeated_postfix_beg) >= tmsocket_postfix.size()
                && std::equal(repeated_postfix_beg,
                              repeated_postfix_beg + tmsocket_postfix.size(),
                              tmsocket_postfix.cbegin()))
            {
                buffer.erase(repeated_postfix_beg, repeated_postfix_beg + tmsocket_postfix.size());
            }
            else if (static_cast<std::size_t>(buffer.cend() - repeated_postfix_beg) >= tmsocket_endmsg.size()
                     && std::equal(repeated_postfix_beg,
                                   repeated_postfix_beg + tmsocket_endmsg.size(),
                                   tmsocket_endmsg.cbegin()))
            {
                msg.assign(buffer.cbegin() + prefix_pos + tmsocket_prefix.size(),
                           buffer.cbegin() + postfix_pos);
                buffer.assign(repeated_postfix_beg + tmsocket_endmsg.size(),
                              buffer.cend());
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

#endif // #ifndef TMSOCKET_DETAILS_PROTOCOL_IPP__
