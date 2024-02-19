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
        auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        clientsMapper.findByPrimaryKey(
            std::atoi(clientId.c_str()),
            [dbClient, clientId, callback](drogon_model::postgres::Clientes client) 
            {
                Json::Value ret;

                // saldo
                Json::Value summary;
                summary["total"] = client.getValueOfSaldo(); 
                summary["limite"] = client.getValueOfLimite();
                summary["data_extrato"] = trantor::Date::date().toCustomedFormattedString("%Y-%m-%d %H:%M:%S", true);
                ret["saldo"] = summary;

                // transacoes
                auto transactionsMapper = drogon::orm::Mapper<drogon_model::postgres::Transacoes>(dbClient);
                transactionsMapper.orderBy("realizada_em", drogon::orm::SortOrder::DESC)
                    .limit(10)
                    .findBy(drogon::orm::Criteria(drogon_model::postgres::Transacoes::Cols::_client_id, drogon::orm::CompareOperator::EQ, clientId),
                        [ret, callback](std::vector<drogon_model::postgres::Transacoes> transactions) mutable
                        {
                            Json::Value lastTransactions;
                            for (const auto& t : transactions) 
                            {
                                auto tAsJson = t.toJson();
                                tAsJson.removeMember("client_id");
                                lastTransactions.append(tAsJson);
                            }
                            ret["ultimas_transacoes"] = lastTransactions;                            
                            auto resp = HttpResponse::newHttpJsonResponse(ret);
                            resp->setStatusCode(HttpStatusCode::k200OK);
                            callback(resp);                            
                        },
                        [callback](const drogon::orm::DrogonDbException& e) 
                        {
                            Json::Value ret;
                            ret["error"] = "Erro obtendo transacoes";
                            auto resp = HttpResponse::newHttpJsonResponse(ret);
                            resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                            callback(resp);                                
                        });
            },
            [callback](const drogon::orm::DrogonDbException& e)  {
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

bool validateRequest(Json::Value& jsonRequest) 
{
    if (!jsonRequest || !jsonRequest.isMember("valor") || !jsonRequest.isMember("tipo") || !jsonRequest.isMember("descricao")) 
    {
        return false;
    }

    double value = jsonRequest["valor"].asDouble();
    std::string type = jsonRequest["tipo"].asString();
    std::string description = jsonRequest["descricao"].asString();

    if (type != "c" && type != "d") 
    {
        return false;
    }

    return true;
}

void RinhaController::processTransaction(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string clientId)
{
    auto jsonRequest = req->getJsonObject();
    if (!validateRequest(*jsonRequest)) 
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