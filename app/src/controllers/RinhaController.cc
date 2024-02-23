#include "RinhaController.h"

#include <models/Clientes.h>
#include <models/Transacoes.h>

#include <sstream>

RinhaController::RinhaController() { std::cout << "RinhaController created. " << std::endl; }

//  curl -i http://localhost:9999/clientes/1/extrato
//  curl http://localhost:9999/clientes/1/extrato | jq
//  curl http://localhost:9999/clientes/1/extrato | jq '.saldo.limite'

void RinhaController::getStatement(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string clientId) {
    int clientIdAsInt;
    try {
        clientIdAsInt = std::stoi(clientId);
    } catch (std::exception const& ex) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
        return;
    }

    if (!checkIfClientExists(clientIdAsInt)) {
        callback(errorResponse_("Cliente nao encontrado", HttpStatusCode::k404NotFound));
        return;
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

//  curl -i http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H 
///    "Content-Type: application/json" 
// curl http://localhost:9999/clientes/1/transacoes -X POST -d '{"valor": 100.0, "tipo": "c", "descricao": "Depósito"}' -H
//      "Content-Type: application/json" | jq

void RinhaController::processTransaction(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string clientId) {
    int clientIdAsInt;
    try {
        clientIdAsInt = std::stoi(clientId);
    } catch (std::exception const& ex) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
        return;
    }

    if (!checkIfClientExists(clientIdAsInt)) {
        callback(errorResponse_("Cliente nao encontrado", HttpStatusCode::k404NotFound));
        return;
    }

    auto jsonRequest = req->getJsonObject();
    if (!jsonRequest || !validateTransactionRequest_(*jsonRequest)) {
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k400BadRequest));
        return;
    }

    auto dbClient = drogon::app().getFastDbClient("default");
    if (!dbClient) {
        callback(errorResponse_("Database nao disponivel", HttpStatusCode::k503ServiceUnavailable));
    } else {
        drogon_model::postgres::Transacoes newTransaction;
        newTransaction.setClientId(std::atoi(clientId.c_str()));
        newTransaction.setValor((*jsonRequest)["valor"].asInt64());
        newTransaction.setTipo((*jsonRequest)["tipo"].asString());
        newTransaction.setDescricao((*jsonRequest)["descricao"].asString());

        auto transactionsMapper = drogon::orm::Mapper<drogon_model::postgres::Transacoes>(dbClient);
        transactionsMapper.insert(
            newTransaction,
            [callback](drogon_model::postgres::Transacoes transaction) {
                Json::Value ret;
                ret["limite"] = transaction.getValueOfLimitePosterior();
                ret["saldo"] = transaction.getValueOfSaldoPosterior();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k200OK);
                callback(resp);
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                callback(errorResponse_("Nao foi possivel completar transacao", HttpStatusCode::k422UnprocessableEntity));
            });
    }
}

// according to the spec the list of clients does not change, so we initialize it once and store it locally
bool RinhaController::checkIfClientExists(int clientId) {
    if (clients_.empty()) {
        auto dbClient = drogon::app().getFastDbClient("default");
        if (!dbClient) {
            throw std::runtime_error("Database nao disponivel");
        }
        auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        clientsMapper.findAll(
            [this](std::vector<drogon_model::postgres::Clientes> clients) {
                for (const auto& c : clients) {
                    clients_.insert(c.getValueOfId());
                }
            },
            [](const drogon::orm::DrogonDbException& e) { throw std::runtime_error("Erro obtendo clientes"); });
    }
    return !(clients_.empty() || clients_.find(clientId) == clients_.end());
}

std::shared_ptr<drogon::HttpResponse> RinhaController::errorResponse_(std::string message, HttpStatusCode status) {
    Json::Value ret;
    ret["error"] = message;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(status);
    return resp;
}

bool RinhaController::validateTransactionRequest_(Json::Value& jsonRequest) {
    try {
        if (!jsonRequest || !jsonRequest.isMember("valor") || !jsonRequest.isMember("tipo") || !jsonRequest.isMember("descricao")) {
            return false;
        }
        if (jsonRequest["valor"].asInt() <= 0) {
            return false;
        }
        std::string type = jsonRequest["tipo"].asString();
        if (type != "c" && type != "d") {
            return false;
        }
        if (jsonRequest["descricao"].asString().empty()) {
            return false;
        }
    } catch (std::exception& e) {
        return false;
    }
    return true;
}
