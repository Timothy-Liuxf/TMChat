noinst_HEADERS +=                           \
    %reldir%/include/client.hpp             \
    ${NOTHING}

bin_PROGRAMS += build/bin/tmchat_client
build_bin_tmchat_client_SOURCES =           \
    %reldir%/cpp/client.cpp                 \
    %reldir%/cpp/main.cpp                   \
    ${NOTHING}

build_bin_tmchat_client_LDADD =             \
    build/lib/libtmsocket.a -lpthread
