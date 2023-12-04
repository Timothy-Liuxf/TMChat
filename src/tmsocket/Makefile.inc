include_tmsocketdir = ${includedir}/tmsocket/include
include_tmsocket_HEADERS =                       \
    %reldir%/include/client_communicator.hpp     \
    %reldir%/include/client_stream.hpp           \
    %reldir%/include/defs.hpp                    \
    %reldir%/include/netexcept.hpp               \
    %reldir%/include/server_communicator.hpp     \
    %reldir%/include/server_stream.hpp           \
    %reldir%/include/tmsocket.hpp                \
    ${NOTHING}

include_tmsocket_detailsdir = ${includedir}/tmsocket/include/details
include_tmsocket_details_HEADERS =               \
    %reldir%/include/details/socket_stream.ipp   \
    ${NOTHING}

lib_LIBRARIES += build/lib/libtmsocket.a
build_lib_libtmsocket_a_SOURCES =                \
    %reldir%/cpp/client_communicator.cpp         \
    %reldir%/cpp/client_stream.cpp               \
    %reldir%/cpp/netexcept.cpp                   \
    %reldir%/cpp/server_communicator.cpp         \
    %reldir%/cpp/server_stream.cpp               \
    %reldir%/cpp/tmsocket.cpp                    \
    ${NOTHING}

noinst_HEADERS +=                                \
    %reldir%/cpp/details/protocol.ipp            \
    %reldir%/cpp/details/socket_impl.ipp         \
    ${NOTHING}
