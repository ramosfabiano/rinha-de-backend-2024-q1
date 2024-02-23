#include "RinhaController.h"

#include <models/Clientes.h>
#include <models/Transacoes.h>

#include <sstream>

//  curl -i http://localhost:9999/clientes/1/extrato
//  curl http://localhost:9999/clientes/1/extrato | jq
//  curl http://localhost:9999/clientes/1/extrato | jq '.saldo.limite'

void RinhaController::getStatement(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string clientId) {
    int clientIdAsInt;
    try {
        clientIdAsInt = std::stoi(clientId);
    } catch (std::exception const& ex) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
    }
    auto dbClient = drogon::app().getFastDbClient("default");
    if (!dbClient) {
        callback(errorResponse_("Database nao disponivel", HttpStatusCode::k503ServiceUnavailable));
    } else {
        auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        clientsMapper.findByPrimaryKey(
            clientIdAsInt,
            [dbClient, clientId, callback](drogon_model::postgres::Clientes client) {
                Json::Value ret;
                // resumo/saldo
                Json::Value summary;
                summary["total"] = client.getValueOfSaldo();
                summary["limite"] = client.getValueOfLimite();
                summary["data_extrato"] = trantor::Date::date().toCustomedFormattedString("%Y-%m-%d %H:%M:%S", true);
                ret["saldo"] = summary;
                // transacoes
                auto transactionsMapper = drogon::orm::Mapper<drogon_model::postgres::Transacoes>(dbClient);
                transactionsMapper.orderBy("realizada_em", drogon::orm::SortOrder::DESC)
                    .limit(10)
                    .findBy(
                        drogon::orm::Criteria(drogon_model::postgres::Transacoes::Cols::_client_id, drogon::orm::CompareOperator::EQ, clientId),
                        [ret, callback](std::vector<drogon_model::postgres::Transacoes> transactions) mutable {
                            Json::Value lastTransactions;
                            for (const auto& t : transactions) {
                                auto tAsJson = t.toJson();
                                tAsJson.removeMember("client_id");
                                lastTransactions.append(tAsJson);
                            }
                            ret["ultimas_transacoes"] = lastTransactions;
                            auto resp = HttpResponse::newHttpJsonResponse(ret);
                            resp->setStatusCode(HttpStatusCode::k200OK);
                            callback(resp);
                        },
                        [callback](const drogon::orm::DrogonDbException& e) {
                            callback(errorResponse_("Erro obtendo transacoes", HttpStatusCode::k500InternalServerError));
                        });
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                callback(errorResponse_("Cliente nao encontrado", HttpStatusCode::k404NotFound));
            });
    }
}

//  curl -i http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H "Content-Type:
//  application/json" curl http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H
//  "Content-Type: application/json" | jq

void RinhaController::processTransaction(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string clientId) {
    auto jsonRequest = req->getJsonObject();
    if (!jsonRequest || !validateTransactionRequest_(*jsonRequest)) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
    }

    auto dbClient = drogon::app().getFastDbClient("default");
    if (!dbClient) {
        callback(errorResponse_("Database nao disponivel", HttpStatusCode::k503ServiceUnavailable));
    } else {
        auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        clientsMapper.findByPrimaryKey(
            std::atoi(clientId.c_str()),
            [dbClient, clientId, jsonRequest, callback](drogon_model::postgres::Clientes client) {
                drogon_model::postgres::Transacoes newTransaction;
                newTransaction.setClientId(std::atoi(clientId.c_str()));
                newTransaction.setValor((*jsonRequest)["valor"].asInt64());
                newTransaction.setTipo((*jsonRequest)["tipo"].asString());
                newTransaction.setDescricao((*jsonRequest)["descricao"].asString());

                auto transactionsMapper = drogon::orm::Mapper<drogon_model::postgres::Transacoes>(dbClient);
                transactionsMapper.insert(
                    newTransaction,
                    [client, callback](drogon_model::postgres::Transacoes transaction) {
                        Json::Value ret;
                        ret["limite"] = client.getValueOfLimite();
                        ret["saldo"] = client.getValueOfSaldo() + transaction.getValueOfValor() * (transaction.getValueOfTipo() == "c" ? 1 : -1);
                        auto resp = HttpResponse::newHttpJsonResponse(ret);
                        resp->setStatusCode(HttpStatusCode::k200OK);
                        callback(resp);
                    },
                    [client, callback](const drogon::orm::DrogonDbException& e) {
                        Json::Value ret;
                        ret["limite"] = client.getValueOfLimite();
                        ret["saldo"] = client.getValueOfSaldo();
                        auto resp = HttpResponse::newHttpJsonResponse(ret);
                        resp->setStatusCode(HttpStatusCode::k422UnprocessableEntity);
                        callback(resp);
                    });
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                callback(errorResponse_("Cliente nao encontrado", HttpStatusCode::k404NotFound));
            });
    }
}

//  curl -i http://localhost:9999/clientes -X POST -d '{"limite": 10000, "saldo": 0}' -H "Content-Type: application/json"
//  curl http://localhost:9999/clientes -X POST -d '{"limite": 10000, "saldo": 0}' -H "Content-Type: application/json" | jq

void RinhaController::addClient(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    auto jsonRequest = req->getJsonObject();
    if (!jsonRequest || !validateNewClientRequest_(*jsonRequest)) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
    }
    auto dbClient = drogon::app().getFastDbClient("default");
    if (!dbClient) {
        callback(errorResponse_("Database nao disponivel", HttpStatusCode::k503ServiceUnavailable));
    } else {
        try {
            drogon_model::postgres::Clientes newClient;
            newClient.setLimite((*jsonRequest)["limite"].asInt64());
            newClient.setSaldo((*jsonRequest)["saldo"].asInt64());

            auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
            clientsMapper.insert(
                newClient,
                [callback](drogon_model::postgres::Clientes client) {
                    std::stringstream errMsg;
                    errMsg << "Client adicionado id=" << client.getValueOfId();
                    callback(errorResponse_(errMsg.str(), HttpStatusCode::k200OK));
                },
                [callback](const drogon::orm::DrogonDbException& e) {
                    callback(errorResponse_("Failed to add client", HttpStatusCode::k422UnprocessableEntity));
                });
        } catch (const std::exception& e) {
            callback(errorResponse_("Falha ao adicionar cliente", HttpStatusCode::k422UnprocessableEntity));
        }
    }
}

std::shared_ptr<drogon::HttpResponse> RinhaController::errorResponse_(std::string message, HttpStatusCode status) {
    Json::Value ret;
    ret["error"] = message;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(status);
    return resp;
}

bool RinhaController::validateNewClientRequest_(Json::Value& jsonRequest) {
    try {
        if (!jsonRequest || !jsonRequest.isMember("limite") || !jsonRequest.isMember("saldo")) {
            return false;
        }
    } catch (std::exception& e) {
        return false;
    }
    return true;
}

bool RinhaController::validateTransactionRequest_(Json::Value& jsonRequest) {
    try {
        if (!jsonRequest || !jsonRequest.isMember("valor") || !jsonRequest.isMember("tipo") || !jsonRequest.isMember("descricao")) {
            return false;
        }
        std::string type = jsonRequest["tipo"].asString();
        if (type != "c" && type != "d") {
            return false;
        }
    } catch (std::exception& e) {
        return false;
    }
    return true;
}
