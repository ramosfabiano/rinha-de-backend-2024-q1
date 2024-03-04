#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RinhaController : public drogon::HttpController<RinhaController, false> {
   private:
    static bool validateTransactionRequest_(Json::Value &jsonRequest);
    static std::shared_ptr<drogon::HttpResponse> errorResponse_(std::string message, HttpStatusCode status);
    static std::string dbNameFromClientId_(std::string clientId);

   public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RinhaController::getStatement, "/clientes/{1}/extrato", drogon::Get);
    ADD_METHOD_TO(RinhaController::processTransaction, "/clientes/{1}/transacoes", drogon::Post);
    METHOD_LIST_END

    void getStatement(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string clientId);
    void processTransaction(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string clientId);

    RinhaController();
};
