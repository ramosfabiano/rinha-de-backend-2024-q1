#include "RinhaController.h"


// https://github.com/drogonframework/drogon/wiki/ENG-03-Quick-Start#dynamic-site
// https://github.com/drogonframework/drogon/wiki/ENG-04-0-Controller-Introduction
// https://github.com/drogonframework/drogon/wiki/ENG-04-2-Controller-HttpController

void RinhaController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    auto hostname = std::getenv("HOSTNAME");
    resp->setStatusCode(HttpStatusCode::k200OK);
    resp->setBody("Hello World from " + std::string(hostname));
    callback(resp);
}
