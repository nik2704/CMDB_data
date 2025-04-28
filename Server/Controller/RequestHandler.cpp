#include "RequestHandler.h"
#include "../View/ResponseFormatter.h"

void RequestHandler::handleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::string_view target = req.target();
    std::string_view api_path = "/api/v1/data";

    if (target.substr(0, api_path.size()) == api_path) {
        std::string_view sub_target = target.substr(api_path.size());
        size_t query_start = sub_target.find('?');

        if (query_start != std::string_view::npos) {
            sub_target = sub_target.substr(0, query_start);
        }

        if (sub_target == "/all") {
            if (req.method() == http::verb::get) {
                handleGetAll(res);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::method_not_allowed, "Метод не разрешен");
            }
        } else if (sub_target == "/level") {
            if (req.method() == http::verb::get) {
                handleGetLevel(req, res);
            } else if (req.method() == http::verb::post) {
                handleAddLevel(req, res);
            } else if (req.method() == http::verb::delete_) {
                handleDeleteLevel(req, res);
            } else if (req.method() == http::verb::patch) {
                handleUpdateLevel(req, res);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::method_not_allowed, "Метод не разрешен");
            }
        } else if (sub_target == "/ci") {
            if (req.method() == http::verb::get) {
                handleGetCi(req, res);
            } else if (req.method() == http::verb::post) {
                handleAddCi(req, res);
            } else if (req.method() == http::verb::delete_) {
                handleDeleteCi(req, res);
            } else if (req.method() == http::verb::patch) {
                handleUpdateCi(req, res);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::method_not_allowed, "Метод не разрешен");
            }
        } else if (sub_target == "/props") {
            if (req.method() == http::verb::get) {
                handleGetProps(req, res);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::method_not_allowed, "Метод не разрешен");
            }
        } else if (sub_target == "/relationship") {
            if (req.method() == http::verb::get) {
                handleGetRelationships(req, res);
            } else if (req.method() == http::verb::post) {
                handleAddRelationships(req, res);
            } else if (req.method() == http::verb::delete_) {
                handleDeleteRelationships(req, res);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::method_not_allowed, "Метод не разрешен");
            }
        } else {
            ResponseFormatter::makeErrorResponse(res, http::status::not_found, "Not found");
        }
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::no_content, "Hello, from the Server!");
    }
}

void RequestHandler::handleGetAll(http::response<http::string_body>& res) {
    json::object records = store_.getAllRecords();
    ResponseFormatter::makeJSONResponse(res, records);
}

void RequestHandler::handleGetLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params = getQueryParams(req);

    if (query_params.count("id")) {
        try {
            int id = std::stoi(query_params["id"]);
            json::object level = store_.getLevel(id);
            
            if(level.empty()) {
                ResponseFormatter::makeErrorResponse(res, http::status::not_found, "Level не найден");
            } else {
                ResponseFormatter::makeJSONResponse(res, level);
            }
        } catch (...) {
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный id");
        }
    } else {
        json::array levels = store_.getAllLevels();
        ResponseFormatter::makeJSONResponse(res, levels);
    }
}

void RequestHandler::handleGetCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params = getQueryParams(req);
    json::array cis = store_.getCi(query_params);

    if (!cis.empty()) {
        ResponseFormatter::makeJSONResponse(res, cis);
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::not_found, "Не найдено");
    }
}

void RequestHandler::handleGetRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params =getQueryParams(req);

    json::array relationships = store_.getRelationships(query_params);

    if (!relationships.empty()) {
        ResponseFormatter::makeJSONResponse(res, relationships);
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::not_found, "Не найдено");
    }
}

void RequestHandler::handleAddLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        auto result = store_.addLevel(json_data);

        if (isResultSuccess(result)) {
            ResponseFormatter::makeJSONResponse(res, result);
        } else {
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Уровень не добавлен");
        }
        
    } catch (...) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный JSON");
    }
}

void RequestHandler::handleAddCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        boost::json::object result;

        if (json_data.is_array()) {
            result = store_.addCis(json_data.as_array());
        } else if (json_data.is_object()) {
            auto result = store_.addCi(json_data.as_object());            
        } else {
            throw std::runtime_error("Не корректный JSON");
        }

        if (isResultSuccess(result)) {
            ResponseFormatter::makeJSONResponse(res, result);
        } else{
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "КЕ не добавлен(ы)");
        }

    } catch (const std::exception& e) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, e.what());
    } catch (...) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный JSON");
    }
}

void RequestHandler::handleAddRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        boost::json::object result;

        if (json_data.is_array()) {
            result = store_.addRelationships(json_data.as_array());
        } else if (json_data.is_object()) {
            result = store_.addRelationship(json_data.as_object());            
        } else {
            throw std::runtime_error("Не корректный JSON data");
        }

        if (isResultSuccess(result)) {
            ResponseFormatter::makeJSONResponse(res, result);
        } else{
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Связи не добавлен(ы)");
        }

    } catch (const std::exception& e) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, e.what());
    } catch (...) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный JSON");
    }    
}

void RequestHandler::handleDeleteLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params = getQueryParams(req);

    if (query_params.count("id")) {
        try {
            int id = std::stoi(query_params["id"]);
            auto result = store_.deleteLevel(id);

            if (isResultSuccess(result)) {
                ResponseFormatter::makeJSONResponse(res, result);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Уровень не удален");
            }
        } catch (...) {
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный id");
        }
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Пропущен id");
    }
}

void RequestHandler::handleDeleteCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params =getQueryParams(req);

    if (query_params.count("id")) {
        try {
            std::string id = query_params["id"];
            auto result = store_.deleteCi(id);

            if (isResultSuccess(result)) {
                ResponseFormatter::makeJSONResponse(res, result);
            } else {
                ResponseFormatter::makeErrorResponse(res, http::status::not_found, "CI не удален");
            }
        } catch (...) {
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный id");
        }
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Пропущен id");
    }
}

void RequestHandler::handleDeleteRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params = getQueryParams(req);

    boost::json::object result = store_.deleteRelationships(query_params);

    ResponseFormatter::makeJSONResponse(res, result);
}

void RequestHandler::handleUpdateCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        boost::json::object result;

        if (json_data.is_array()) {
            result = store_.updateCis(json_data.as_array());
        } else if (json_data.is_object()) {
            result = store_.updateCi(json_data.as_object());            
        } else {
            throw std::runtime_error("Не корректный JSON");
        }

        if (isResultSuccess(result)) {
            ResponseFormatter::makeJSONResponse(res, result);
        } else{
            ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "КЕ не обновлен(ы)");
        }

    } catch (const std::exception& e) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, e.what());
    } catch (...) {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Не корректный JSON");
    }
}

void RequestHandler::handleUpdateLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    auto json_data = json::parse(req.body()).as_object();
    auto result = store_.updateLevel(json_data);

    if (isResultSuccess(result)) {
        ResponseFormatter::makeJSONResponse(res, result);
    } else {
        ResponseFormatter::makeErrorResponse(res, http::status::bad_request, "Уровень не обновлен");
    }

}

std::map<std::string, std::string> RequestHandler::getQueryParams(http::request<http::string_body>& req) {
    std::map<std::string, std::string> query_params;
    std::string_view target = req.target();
    size_t query_start = target.find('?');

    if (query_start != std::string_view::npos) {
        std::string query_string(target.substr(query_start + 1));
        std::stringstream ss(query_string);
        std::string item;

        while (std::getline(ss, item, '&')) {
            size_t equals_pos = item.find('=');
            if (equals_pos != std::string::npos) {
                query_params[item.substr(0, equals_pos)] = item.substr(equals_pos + 1);
            }
        }
    }

    return query_params;
}

bool RequestHandler::isResultSuccess(json::object& result) {
    if (!result.contains("status")) {
        return true;
    }

    std::string status = boost::json::value_to<std::string>(result.at("status"));
    if (status != "success") return false;

    return true;
}

void RequestHandler::handleGetProps(http::request<http::string_body>& /*req*/, http::response<http::string_body>& res) {
    boost::json::object result;

    result = store_.getPropsList();
    ResponseFormatter::makeJSONResponse(res, result);    
}
