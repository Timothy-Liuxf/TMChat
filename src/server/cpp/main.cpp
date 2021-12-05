#include <tmsocket/include/server_stream.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

int main(void)
{
    std::cout << "Please input server port: " << std::flush;
    std::string port;
    std::getline(std::cin, port);
    tmsocket::server_stream ss;
    std::vector<int> client_fds;
    std::mutex mtx;
    ss.add_log([](const std::string& str) { std::cout << str << std::endl; });
    ss.on_listen([] { std::cout << "Server begins to listen!" << std::endl; });
    ss.on_connect
    (
        [&](int fd)
        {
            std::unique_lock<std::mutex> lock(mtx);
            client_fds.emplace_back(fd);
            std::cout << "A client successfully connected!" << std::endl;
        }
    );
    ss.on_reveive([](const std::string& str) { std::cout << "Received: " << str << std::endl; });

    bool finished_listen = false;
    std::mutex finished_listen_mtx;
    std::condition_variable cond;

    ss.on_listen([&]
    {
        std::unique_lock<std::mutex> lock(finished_listen_mtx);
        finished_listen = true;
        cond.notify_all();
    });

    std::thread thr
    {
        [&]
        {

            {
                std::unique_lock<std::mutex> lock(finished_listen_mtx);
                cond.wait(lock, [&] { return finished_listen; });
            }

            for (int i = 0; i < 10; ++i)
            {
                ss.send_to_all_clients("All clients!\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            ::std::cout << "End communication!" << std::endl;
            ss.end_communication();
        }
    };

    ss.listen("", port);

    thr.join();
    return 0;
}
