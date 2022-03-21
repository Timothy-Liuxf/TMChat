include_prepdir = ${includedir}/prep/include
include_prep_HEADERS =                              \
    %reldir%/include/compiler.h                     \
    %reldir%/include/concurrent_list.hpp            \
    %reldir%/include/concurrent_queue.hpp           \
    %reldir%/include/cpp_std.h                      \
    %reldir%/include/event.hpp                      \
    %reldir%/include/namespace.hpp                  \
    %reldir%/include/os.h                           \
    %reldir%/include/os_net.h                       \
    %reldir%/include/os_net_defs.h                  \
    %reldir%/include/prep.h                         \
    %reldir%/include/semaphore.hpp                  \
    ${NOTHING}

include_prep_detailsdir = ${includedir}/prep/include/details
include_prep_details_HEADERS =                                      \
    %reldir%/include/details/compiler_clang.i                       \
    %reldir%/include/details/compiler_gcc.i                         \
    %reldir%/include/details/compiler_msvc.i                        \
    ${NOTHING}
