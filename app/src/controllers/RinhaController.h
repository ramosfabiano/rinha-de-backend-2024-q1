#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RinhaController : public drogon::HttpController<RinhaController, false> {
   private:
    bool somePrivateParameter;

    static bool validateNewClientRequest_(Json::Value &jsonRequest);
    static bool validateTransactionRequest_(Json::Value &jsonRequest);
    static std::shared_ptr<drogon::HttpResponse> errorResponse_(std::string message, HttpStatusCode status);

   public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RinhaController::getStatement, "/clientes/{1}/extrato", drogon::Get);
    ADD_METHOD_TO(RinhaController::processTransaction, "/clientes/{1}/transacoes", drogon::Post);
    // to support testing, not part of the original requirements
    ADD_METHOD_TO(RinhaController::addClient, "/clientes", drogon::Post);
    METHOD_LIST_END

    void getStatement(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string clientId);
    void processTransaction(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string clientId);
    void addClient(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    RinhaController() { std::cout << "RinhaController created. " << std::endl; }
};
