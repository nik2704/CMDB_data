#include "RequestHandler.h"
#include "../View/ResponseFormatter.h"

void RequestHandler::HandleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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
                HandleGetAll(res);
            } else {
                ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
            }
        } else if (sub_target == "/level") {
            if (req.method() == http::verb::get) {
                HandleGetLevel(req, res);
            } else if (req.method() == http::verb::post) {
                HandleAddLevel(req, res);
            } else if (req.method() == http::verb::delete_) {
                HandleDeleteLevel(req, res);
            } else if (req.method() == http::verb::patch) {
                HandleUpdateLevel(req, res);
            } else {
                ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
            }
        } else if (sub_target == "/ci") {
            if (req.method() == http::verb::get) {
                HandleGetCi(req, res);
            } else if (req.method() == http::verb::post) {
                HandleAddCi(req, res);
            } else if (req.method() == http::verb::delete_) {
                HandleDeleteCi(req, res);
            } else if (req.method() == http::verb::patch) {
                HandleUpdateCi(req, res);
            } else {
                ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
            }
        } else if (sub_target == "/relationship") {
            if (req.method() == http::verb::get) {
                HandleGetRelationships(req, res);
            } else if (req.method() == http::verb::post) {
                HandleAddRelationships(req, res);
            } else if (req.method() == http::verb::delete_) {
                HandleDeleteRelationships(req, res);
            } else {
                ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
            }
        } else {
            ResponseFormatter::MakeErrorResponse(res, http::status::not_found, "Not found");
        }
    } else {
        ResponseFormatter::MakeErrorResponse(res, http::status::no_content, "Hello, from the Server!");
    }
}

void RequestHandler::HandleGetAll(http::response<http::string_body>& res) {
    json::object records = store_.GetAllRecords();
    ResponseFormatter::MakeJSONResponse(res, records);
}

void RequestHandler::HandleGetLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::map<std::string, std::string> query_params = getQueryParams(req);

    if (query_params.count("id")) {
        try {
            int id = std::stoi(query_params["id"]);
            json::object level = store_.GetLevel(id);
            ResponseFormatter::MakeJSONResponse(res, level);
        } catch (...) {
            ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid id");
        }
    } else {
        json::array levels = store_.GetAllLevels();
        ResponseFormatter::MakeJSONResponse(res, levels);
    }
}

void RequestHandler::HandleGetCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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
    json::array cis = store_.GetCi(query_params);
    ResponseFormatter::MakeJSONResponse(res, cis);
}

void RequestHandler::HandleGetRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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
    json::array relationships = store_.GetRelationships(query_params);
    ResponseFormatter::MakeJSONResponse(res, relationships);
}

void RequestHandler::HandleAddLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        store_.AddLevel(json_data);
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandleAddCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        if (json_data.is_array()) {
            store_.AddCis(json_data.as_array());
        } else if (json_data.is_object()) {
            store_.AddCi(json_data.as_object());
        } else {
            throw std::runtime_error("Invalid JSON data");
        }
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandleAddRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_array();
        store_.AddRelationships(json_data);
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandleDeleteLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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

    if (query_params.count("id")) {
        try {
            int id = std::stoi(query_params["id"]);
            store_.DeleteLevel(id);
            ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
        } catch (...) {
            ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid id");
        }
    } else {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Missing id");
    }
}

void RequestHandler::HandleDeleteCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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
    store_.DeleteCi(query_params);
    ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
}

void RequestHandler::HandleDeleteRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res) {
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
    store_.DeleteRelationships(query_params);
    ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
}

void RequestHandler::HandleUpdateCi(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        if (json_data.is_array()) {
            store_.UpdateCis(json_data.as_array());
        } else if (json_data.is_object()) {
            store_.UpdateCi(json_data.as_object());
        } else {
            throw std::runtime_error("Invalid JSON data");
        }
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandleUpdateLevel(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        store_.UpdateLevel(json_data);
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}});
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandlePostRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        // Реализация добавления данных в хранилище (store_)
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}}); // Явно указываем тип
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandlePutRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        // Реализация обновления данных в хранилище (store_)
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}}); // Явно указываем тип
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandlePatchRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        // Реализация частичного обновления данных в хранилище (store_)
        ResponseFormatter::MakeJSONResponse(res, json::object{{"status", "success"}}); // Явно указываем тип
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
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
// void RequestHandler::HandleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {

//     std::string_view target = req.target();
//     std::string_view api_path = "/api/v1/data";

//     if (target.substr(0, api_path.size()) == api_path &&
//         (target.size() == api_path.size() || target[api_path.size()] == '?')) {
        
//         if (req.method() == http::verb::get) {
//             HandleGetRequest(res);
//         } else if (req.method() == http::verb::post) {
//             HandlePostRequest(req, res);
//         } else if (req.method() == http::verb::put) {
//             HandlePutRequest(req, res);
//         } else if (req.method() == http::verb::patch) {
//             HandlePatchRequest(req, res);
//         } else {
//             ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
//         }

//     } else {
//         ResponseFormatter::MakeErrorResponse(res, http::status::no_content, "Hello, from the Server!");
//     }
// }

// void RequestHandler::HandleGetRequest(http::response<http::string_body>& res) {
//     json::object records = store_.GetAllRecords();
//     ResponseFormatter::MakeJSONResponse(res, records);
// }

// void RequestHandler::HandlePostRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
//     try {
//         auto json_data = json::parse(req.body()).as_object();
//         int new_id = store_.AddRecord(json_data);
//         ResponseFormatter::MakeJSONResponse(res, { {"status", "success"}, {"id", new_id} });
//     } catch (...) {
//         ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
//     }
// }

// void RequestHandler::HandlePutRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
//     try {
//         auto json_data = json::parse(req.body()).as_object();
//         int id = std::stoi(std::string(req.target().substr(req.target().find_last_of('/') + 1)));
//         bool updated = store_.UpdateRecord(id, json_data);
//         if (updated) {
//             ResponseFormatter::MakeJSONResponse(res, { {"status", "success"}, {"id", id} });
//         } else {
//             ResponseFormatter::MakeErrorResponse(res, http::status::not_found, "Record not found");
//         }
//     } catch (...) {
//         ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data or ID");
//     }
// }

// void RequestHandler::HandlePatchRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
//     try {
//         auto json_data = json::parse(req.body());
//         auto update = json_data.as_object().at("update").as_object();
//         bool updated = false;
//         for (auto& [id, record] : store_.GetAllRecords()) {
//             for (auto& [key, value] : update) {
//                 store_.UpdateRecord(std::stoi(id), update);
//             }
//             updated = true;
//         }
//         ResponseFormatter::MakeJSONResponse(res, updated ? json::object({{"status", "success"}}) : json::object({{"error", "No records updated"}}));
//     } catch (...) {
//         ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
//     }
// }
