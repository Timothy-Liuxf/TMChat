#include <tmsocket/include/client_stream.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

int main(void)
{
    std::cout << "Please input server ip: " << std::flush;
    std::string host;
    std::cin >> host;
    std::cout << "Please input server port: " << std::flush;
    std::string port;
    std::cin >> port;
    tmsocket::client_stream cs;


    cs.add_log([](const std::string& str) { std::cout << str << std::endl; });
    cs.on_connect
    (
        [&]()
        {
            std::cout << "Successfully connected to server!" << std::endl;
        }
    );
    cs.on_reveive([](const std::string& str) { std::cout << str << std::endl; });

    bool finished_connect = false;
    std::mutex finished_connect_mtx;
    std::condition_variable cond;

    cs.on_connect([&]
    {
        std::unique_lock<std::mutex> lock(finished_connect_mtx);
        finished_connect = true;
        cond.notify_all();
    });

    std::thread thr
    {
        [&]
        {

            {
                std::unique_lock<std::mutex> lock(finished_connect_mtx);
                cond.wait(lock, [&] { return finished_connect; });
            }

            for (int i = 0; i < 10; ++i)
            {
                cs.send_to_server("All clients!\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            cs.end_communication();
        }
    };

    cs.connect(host, port);

    thr.join();
    return 0;
}

