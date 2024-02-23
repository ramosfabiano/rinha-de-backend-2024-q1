#include <controllers/RinhaController.h>
#include <drogon/drogon.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace drogon;
using namespace drogon::orm;

using Callback = std::function<void(const HttpResponsePtr &)>;

int main(int argc, const char **argv) {
    // allow for docker-compose to properly start the database
    std::this_thread::sleep_for(std::chrono::seconds(3));

    app().loadConfigFile("/etc/rinha/config.json");
    drogon::app().registerController(std::make_shared<RinhaController>());
    app().run();
    return 0;
}