#include "../include/server.hpp"
#include "../include/local_database.hpp"
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

int main(void)
{
    ::tmchat::server sv
    (
        []() -> ::std::unique_ptr<::tmchat::i_database<::tmchat::server::data_type>>
        {
            return ::std::unique_ptr<::tmchat::local_database>(new ::tmchat::local_database());
        }
    );
    return 0;
}
