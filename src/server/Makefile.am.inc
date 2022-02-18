noinst_HEADERS +=                           \
    %reldir%/include/i_database.hpp         \
    %reldir%/include/local_database.hpp     \
    %reldir%/include/server.hpp             \
    ${NOTHING}

bin_PROGRAMS += build/bin/tmchat_server
build_bin_tmchat_server_SOURCES =           \
    %reldir%/cpp/main.cpp                   \
    %reldir%/cpp/server.cpp                 \
    ${NOTHING}

build_bin_tmchat_server_LDADD =             \
    build/lib/libtmsocket.a -lpthread
