#include "RequestHandler.h"
#include "../View/ResponseFormatter.h"

void RequestHandler::HandleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    if (req.method() == http::verb::get) {
        HandleGetRequest(res);
    } else if (req.method() == http::verb::post) {
        HandlePostRequest(req, res);
    } else if (req.method() == http::verb::put) {
        HandlePutRequest(req, res);
    } else if (req.method() == http::verb::patch) {
        HandlePatchRequest(req, res);
    } else {
        ResponseFormatter::MakeErrorResponse(res, http::status::method_not_allowed, "Method not allowed");
    }
}

void RequestHandler::HandleGetRequest(http::response<http::string_body>& res) {
    json::object records = store_.GetAllRecords();
    ResponseFormatter::MakeJSONResponse(res, records);
}

void RequestHandler::HandlePostRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        int new_id = store_.AddRecord(json_data);
        ResponseFormatter::MakeJSONResponse(res, { {"status", "success"}, {"id", new_id} });
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}

void RequestHandler::HandlePutRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body()).as_object();
        int id = std::stoi(std::string(req.target().substr(req.target().find_last_of('/') + 1)));
        bool updated = store_.UpdateRecord(id, json_data);
        if (updated) {
            ResponseFormatter::MakeJSONResponse(res, { {"status", "success"}, {"id", id} });
        } else {
            ResponseFormatter::MakeErrorResponse(res, http::status::not_found, "Record not found");
        }
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data or ID");
    }
}

void RequestHandler::HandlePatchRequest(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        auto json_data = json::parse(req.body());
        auto update = json_data.as_object().at("update").as_object();
        bool updated = false;
        for (auto& [id, record] : store_.GetAllRecords()) {
            for (auto& [key, value] : update) {
                store_.UpdateRecord(std::stoi(id), update);
            }
            updated = true;
        }
        ResponseFormatter::MakeJSONResponse(res, updated ? json::object({{"status", "success"}}) : json::object({{"error", "No records updated"}}));
    } catch (...) {
        ResponseFormatter::MakeErrorResponse(res, http::status::bad_request, "Invalid JSON data");
    }
}
