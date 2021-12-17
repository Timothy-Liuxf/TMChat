#ifndef PREP_COMPILER_H__
#   error Please include <prep/compiler.h> instead.
#endif

// C++ Standard

#ifdef __cplusplus

#   if __cplusplus < 201103L
#       error Only C++11 or later versions of C++ is supported.
#   endif

#   if __cplusplus >= 201103L
#       define PREP_HAS_CXX11
#   endif

#   if __cplusplus >= 201402L
#       define PREP_HAS_CXX14
#   endif

#   if __cplusplus >= 201703L
#       define PREP_HAS_CXX17
#   endif

#endif // #ifdef __cplusplus

// Attributes

#define PREP_FORCED_INLINE __attribute__((__always_inline__)) __inline__
#define PREP_NODISCARD __attribute__((__warn_unused_result__))
#define PREP_DECLARE_UNUSED_VARIABLE(x) ((void)(x))

// Reference static library

#define PREP_REFERENCE_STATIC_LIB(libname)
