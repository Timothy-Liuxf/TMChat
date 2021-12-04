#ifndef PREP_CPP_STD_H__
#define PREP_CPP_STD_H__

#include <prep/include/compiler.h>

#ifdef __cplusplus

#   define PREP_CONSTEXPR constexpr inline

#   if defined(PREP_HAS_CXX14)
#       define PREP_CXX14_CONSTEXPR constexpr inline
#   else
#       define PREP_CXX14_CONSTEXPR inline
#   endif

#   if defined(PREP_HAS_CXX17)
#       define PREP_CONSTEXPR_IF if constexpr
#   else
#       define PREP_CONSTEXPR_IF if
#   endif

#   define PREP_EXTERN_C extern "C"
#   define PREP_EXTERN_C_BEGIN extern "C" {
#   define PREP_EXTERN_C_END }

#endif // #ifdef __cplusplus

#endif
