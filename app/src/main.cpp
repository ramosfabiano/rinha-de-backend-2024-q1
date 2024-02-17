#include <iostream>
#include <cstdlib>
#include <drogon/drogon.h>

using namespace drogon;

int main (int argc, const char **argv)
{
    const auto port = std::getenv("APP_PORT") ? std::atoi(std::getenv("APP_PORT")) : 8080;
    std::cout << "Listening at port " << port << std::endl;

    app()
        .setLogPath("./")
        .setLogLevel(trantor::Logger::kDebug)
        .addListener("0.0.0.0", port)
        .setThreadNum(8);

    //app().registerController(controller);

    app().run();

    std::cout << "Exiting." << std::endl;

    return 0;
}