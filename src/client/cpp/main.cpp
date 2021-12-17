#include "../include/client.hpp"
#include <tmsocket/include/tmsocket.hpp>
#include <iostream>
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
            ::tmchat::client cl;
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
