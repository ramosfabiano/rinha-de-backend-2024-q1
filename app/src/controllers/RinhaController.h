#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RinhaController : public drogon::HttpController<RinhaController, false>
{
  private:
    bool somePrivateParameter;

    bool validateTransactionRequest_(Json::Value& jsonRequest);
    static std::shared_ptr<drogon::HttpResponse> errorResponse_(std::string message, HttpStatusCode status);


  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(User::get, "/{2}/{1}", Get); // path is /demo/v1/User/{arg2}/{arg1}
    // METHOD_ADD(User::your_method_name, "/{1}/{2}/list", Get); // path is /demo/v1/User/{arg1}/{arg2}/list
    // ADD_METHOD_TO(User::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list      
      ADD_METHOD_TO(RinhaController::getStatement,"/clientes/{1}/extrato", drogon::Get);
      ADD_METHOD_TO(RinhaController::processTransaction,"/clientes/{1}/transacoes", drogon::Post);
    METHOD_LIST_END

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void getStatement(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId);
    void processTransaction(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId);

    RinhaController(bool someParameter) : somePrivateParameter(someParameter) { std::cout << "RinhaController created. " << std::endl; }
};
