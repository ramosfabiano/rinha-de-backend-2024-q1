#include "RinhaController.h"


// https://github.com/drogonframework/drogon/wiki/ENG-03-Quick-Start#dynamic-site
// https://github.com/drogonframework/drogon/wiki/ENG-04-0-Controller-Introduction
// https://github.com/drogonframework/drogon/wiki/ENG-04-2-Controller-HttpController

//  curl -i http://localhost:9999/clientes/1/extrato

// https://github.com/zanfranceschi/rinha-de-backend-2024-q1/tree/main?tab=readme-ov-file#transa%C3%A7%C3%B5es

void RinhaController::getStatement(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId)
{
    Json::Value ret;
    ret["cliente"]= __FUNCTION__;
    ret["message"]="Processing cliente " + clientId + " from " + std::string(std::getenv("HOSTNAME"));

    auto dbClient = drogon::app().getDbClient();
    if (!dbClient) 
    {
        ret["result"] = "database not available";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k503ServiceUnavailable);
        callback(resp);
    }
    else
    {
        ret["result"] = "ok";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    }
}

//  curl -i http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H "Content-Type: application/json"

// https://github.com/zanfranceschi/rinha-de-backend-2024-q1/tree/main?tab=readme-ov-file#extrato

void RinhaController::processTransaction(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId)
{
    Json::Value ret;
    ret["cliente"]= __FUNCTION__;
    ret["message"]="Processing cliente " + clientId + " from " + std::string(std::getenv("HOSTNAME"));

    auto json = req->getJsonObject();
    if (!json || !json->isMember("valor") || !json->isMember("tipo") || !json->isMember("descricao")) 
    {
        ret["result"] = "invalid request";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    double value = (*json)["valor"].asDouble();
    std::string type = (*json)["tipo"].asString();
    std::string description = (*json)["descricao"].asString();

    if (type != "c" && type != "d") 
    {
        ret["result"] = "invalid request";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // TODO: process transaction

    ret["result"]="ok";
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
}