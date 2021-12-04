#ifndef PROP_COMPILER_H__
#   error Please include <prep/compiler.h> instead.
#endif

// C++ Standard

#ifdef __cplusplus

#   if _MSVC_LANG < 201402L
#       error Only /std:c++14 or later options of C++ language version is supported on MSVC.
#   endif

#   define PRIOR_HAS_CXX11

#   if _MSVC_LANG >= 201402L
#       define PRIOR_HAS_CXX14
#   endif

#   if _MSVC_LANG >= 201703L
#       define PRIOR_HAS_CXX17
#   endif

#endif // #ifdef __cplusplus

// Attributes

#define PRIOR_FORCED_INLINE __forceinline
#define PRIOR_NODISCARD _Check_return_
#define PRIOR_DECLARE_UNUSED_VARIABLE(x) ((void)(x))

// Reference static library

#define REFERENCE_STATIC_LIB(libname) __pragma(comment(lib, #libname".lib"))
