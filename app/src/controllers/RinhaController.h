#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;


// https://github.com/drogonframework/drogon/wiki/ENG-03-Quick-Start#dynamic-site
// https://github.com/drogonframework/drogon/wiki/ENG-04-0-Controller-Introduction
// https://github.com/drogonframework/drogon/wiki/ENG-04-2-Controller-HttpController

class RinhaController : public drogon::HttpSimpleController<RinhaController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    //PATH_ADD("/test", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/test", Get);
    PATH_LIST_END
};
