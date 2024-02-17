#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <drogon/drogon.h>

using namespace drogon;

int main (int argc, const char **argv)
{
    // APP_PORT env
    auto port = std::getenv("APP_PORT") ? std::atoi(std::getenv("APP_PORT")) : 8080;

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Listening at port " << port << std::endl;
    }

    return 0;
}
