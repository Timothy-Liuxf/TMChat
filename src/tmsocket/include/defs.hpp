#ifndef TMSOCKET_DEFS_HPP__
#define TMSOCKET_DEFS_HPP__

#include <prep/include/prep.h>

#define TMSOCKET_NAMESPACE_BEGIN namespace tmsocket {
#define TMSOCKET_NAMESPACE_END }

#if defined(_WIN32)
PREP_REFERENCE_STATIC_LIB(Ws2_32);
#endif

#endif
