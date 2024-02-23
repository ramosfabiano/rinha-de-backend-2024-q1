#include "RinhaController.h"

#include <models/Clientes.h>
#include <models/Transacoes.h>

#include <sstream>

RinhaController::RinhaController() { LOG_INFO << "RinhaController created. "; }

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

    // check cache
    auto cachedValue = getFromRedisCache(clientId);
    if (!cachedValue.empty()) {
        Json::Value ret;
        std::stringstream ss;
        ss << base64Decode_(cachedValue);
        ss >> ret;
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
        return;
    }    

    auto dbClient = drogon::app().getFastDbClient("default");
    if (!dbClient) {
        callback(errorResponse_("Database nao disponivel", HttpStatusCode::k503ServiceUnavailable));
    } else {
        auto clientsMapper = drogon::orm::Mapper<drogon_model::postgres::Clientes>(dbClient);
        clientsMapper.findByPrimaryKey(
            clientIdAsInt,
            [this, dbClient, clientId, callback](drogon_model::postgres::Clientes client) {
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
                        [this, ret, clientId, callback](std::vector<drogon_model::postgres::Transacoes> transactions) mutable {
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
                            // update redis cache 
                            setInRedisCache(clientId, base64Encode_(ret.toStyledString()));
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
        callback(errorResponse_("Requisicao invalida", HttpStatusCode::k422UnprocessableEntity));
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
            [this, clientId, callback](drogon_model::postgres::Transacoes transaction) {
                Json::Value ret;
                ret["limite"] = transaction.getValueOfLimitePosterior();
                ret["saldo"] = transaction.getValueOfSaldoPosterior();
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k200OK);
                callback(resp);
                // invalidate cache
                deleteFromRedisCache(clientId);                         
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

std::string RinhaController::getFromRedisCache(const std::string& clientId)
{
    try {
        auto keyValue = app().getRedisClient()->execCommandSync<std::string>(
            [](const drogon::nosql::RedisResult &r) {
                if (r.isNil())
                {
                    return std::string();
                }
                return r.asString();
            },
            "get %s", clientId.c_str());
        LOG_DEBUG << "Got key from redis: " << clientId;
        return keyValue;
    } catch (const std::exception &e) {
        LOG_ERROR << "Failed getting data from redis!!! " << e.what();
    }
    return std::string();
}

void RinhaController::setInRedisCache(const std::string& clientId, const std::string& value)
{
    std::string redisCommand = "set " + clientId + " " + value;
    try {
        auto redisOk = app().getRedisClient()->execCommandSync<bool>(
            [](const drogon::nosql::RedisResult &r) {
                return !r.isNil(); 
            },
            redisCommand.c_str());
        LOG_DEBUG << (redisOk? "Added " : "Failed to add ") << "key to redis: " << clientId;
    } 
    catch (const std::exception &e) {
        LOG_ERROR << "Error adding data to redis!!! " << e.what();
    }
}

void RinhaController::deleteFromRedisCache(const std::string& clientId)
{
    try {
        auto keyCount = app().getRedisClient()->execCommandSync(
            [](const drogon::nosql::RedisResult &r) {
                return r.asInteger();
            },
            "del %s", clientId.c_str());
        LOG_DEBUG << "Deleted key from redis: " << clientId;
    } 
    catch (const std::exception &e) {
        LOG_ERROR << "Error deleting data from redis!!! " << e.what();
    }       
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

std::shared_ptr<drogon::HttpResponse> RinhaController::errorResponse_(std::string message, HttpStatusCode status) {
    Json::Value ret;
    ret["error"] = message;
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(status);
    return resp;
}

std::string RinhaController::base64Encode_(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;    
}

std::string RinhaController::base64Decode_(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}