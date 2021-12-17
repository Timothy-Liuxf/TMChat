#include "../include/server.hpp"
#include "../include/local_database.hpp"
#include <tmsocket/include/tmsocket.hpp>
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
    try
    {
        ::tmsocket::tmsocket::initialize_network();
        try
        {
            ::tmchat::server sv
            (
                []() -> ::std::unique_ptr<::tmchat::i_database<::tmchat::server::data_type>>
                {
                    return ::std::unique_ptr<::tmchat::local_database>(new ::tmchat::local_database());
                }
            );
            ::tmsocket::tmsocket::end_network();
        }
        catch (...)
        {
            ::tmsocket::tmsocket::end_network();
            throw;
        }
    }
    catch (::std::exception& e)
    {
        ::std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
