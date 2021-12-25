#ifndef TMCHAT_H__
#define TMCHAT_H__

#ifdef __cplusplus

#   define TMCHAT_NAMESPACE_BEGIN namespace tmchat {
#   define TMCHAT_NAMESPACE_END }

#   include <cstdint>

namespace tmchat
{
    using default_id_type = ::std::uint32_t;
}

#else // #ifdef __cplusplus

#   define TMCHAT_NAMESPACE_BEGIN
#   define TMCHAT_NAMESPACE_END

#   include <stdint.h>

typedef uint32_t default_id_type;

#endif

#endif // #ifdef TMCHAT_H__
