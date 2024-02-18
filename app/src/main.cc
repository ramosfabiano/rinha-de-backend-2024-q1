#include <iostream>
#include <cstdlib>
#include <drogon/drogon.h>
#include <controllers/RinhaController.h>

using namespace drogon;
using namespace drogon::orm;

using Callback = std::function<void (const HttpResponsePtr &)> ;

int main (int argc, const char **argv)
{
    app().loadConfigFile("/etc/rinha/config.json");

    /*
    app().registerHandler("/", [](const HttpRequestPtr& req, Callback &&callback)
    {
        auto resp = HttpResponse::newHttpResponse();
        auto hostname = std::getenv("HOSTNAME");
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setBody("Hello World from " + std::string(hostname));
        callback(resp);
    });
    */

    app().run();

    std::cout << "Exiting." << std::endl;

    return 0;
}