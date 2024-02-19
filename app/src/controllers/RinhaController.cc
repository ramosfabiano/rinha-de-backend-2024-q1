#include "RinhaController.h"
#include <models/Clientes.h>
#include <models/Transacoes.h>
#include <sstream>

//  curl -i http://localhost:9999/clientes/1/extrato
//  curl http://localhost:9999/clientes/1/extrato | jq

// https://github.com/zanfranceschi/rinha-de-backend-2024-q1/tree/main?tab=readme-ov-file#transa%C3%A7%C3%B5es


void RinhaController::getStatement(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId)
{
    auto dbClient = drogon::app().getFastDbClient();
    if (!dbClient) 
    {  
        Json::Value ret;
        ret["error"] = "Database nao disponivel";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k503ServiceUnavailable);
        callback(resp);              
    }
    else
    {
        auto mapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        mapper.findByPrimaryKey(
            std::atoi(clientId.c_str()),
            [clientId, callback](drogon_model::postgres::Clientes client) {
                Json::Value ret;

                // saldo
                Json::Value saldo;
                saldo["total"] = client.getValueOfSaldo(); 
                saldo["data_extrato"] = trantor::Date::date().toCustomedFormattedString("%Y-%m-%d %H:%M:%S", true);
                saldo["limite"] = client.getValueOfLimite();
                ret["saldo"] = saldo;

                // transacoes
                Json::Value transacoes;
                //auto transacoesMapper = drogon::orm::Mapper<drogon_model::postgres::Transacoes>(client.getDbClient());
                { // TODO: do the real thing
                    Json::Value transacao1;
                    // TODO: convert to json automatically from the model 
                    transacao1["valor"] = 10;
                    transacao1["tipo"] = "c";
                    transacao1["descricao"] = "descricao";
                    transacao1["realizada_em"] = "2024-01-17T02:34:38.543030Z";
                    transacoes.append(transacao1);
                }
                ret["ultimas_transacoes"] = transacoes;

                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k200OK);
                callback(resp);
            },
            [clientId, callback](const drogon::orm::DrogonDbException& e)  {
                Json::Value ret;
                ret["error"] = "Cliente nao encontrado";
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k404NotFound);
                callback(resp);          
            }
        );
    }
}

//  curl -i http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H "Content-Type: application/json"
//  curl http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H "Content-Type: application/json" | jq

// https://github.com/zanfranceschi/rinha-de-backend-2024-q1/tree/main?tab=readme-ov-file#extrato

void RinhaController::processTransaction(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId)
{
    //
    // TODO: validate with the model, not manually
    //
    // TODO: implement properly
    //

    auto json = req->getJsonObject();
    if (!json || !json->isMember("valor") || !json->isMember("tipo") || !json->isMember("descricao")) 
    {
        Json::Value ret;
        ret["error"] = "invalid request";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);         
    }

    double value = (*json)["valor"].asDouble();
    std::string type = (*json)["tipo"].asString();
    std::string description = (*json)["descricao"].asString();

    if (type != "c" && type != "d") 
    {
        Json::Value ret;
        ret["error"] = "invalid request";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp); 
    }

        Json::Value ret;
        ret["error"] = "not implemented yet";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k501NotImplemented);
        callback(resp); 
}