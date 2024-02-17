#include <iostream>
#include <cstdlib>
#include <drogon/drogon.h>

using namespace drogon;


std::string getEnv_(std::string const & key) 
{
    char * val;
    val = std::getenv(key.c_str());
    return val == NULL ? std::string("") : std::string(val);
}


int main (int argc, const char **argv)
{
    const auto port = std::atoi(getEnv_("APP_PORT").c_str());

    std::cout << "Listening at port " << port << std::endl;

    const auto postgresHost = getEnv_("POSTGRES_HOST");
    const auto postgresPort = getEnv_("POSTGRES_PORT").c_str();
    const auto postgresDBName = getEnv_("POSTGRES_DB");
    const auto postgresUser = getEnv_("POSTGRES_USER");
    const auto postgresPassword = getEnv_("POSTGRES_PASSWORD");

    std::cout << "Postgres Host: " << postgresHost << std::endl;
    std::cout << "Postgres Port: " << postgresPort << std::endl;
    std::cout << "Postgres DB Name: " << postgresDBName << std::endl;
    std::cout << "Postgres User: " << postgresUser << std::endl;
    std::cout << "Postgres Password: " << postgresPassword << std::endl;

    std::cout << "Connecting to Postgres..." << std::endl;
    // TODO: create db client


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