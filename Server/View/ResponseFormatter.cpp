#include "ResponseFormatter.h"

void ResponseFormatter::makeJSONResponse(http::response<http::string_body>& res, const json::object& obj) {
    res.set(http::field::content_type, "application/json");
    res.body() = json::serialize(obj);
    res.prepare_payload();
}

void ResponseFormatter::makeJSONResponse(http::response<http::string_body>& res, const json::array& arr) {
    res.set(http::field::content_type, "application/json");
    res.body() = json::serialize(arr);
    res.prepare_payload();
}

void ResponseFormatter::makeErrorResponse(http::response<http::string_body>& res, http::status status, const std::string& message) {
    res.result(status);
    res.set(http::field::content_type, "application/json");
    res.body() = json::serialize(json::object({{"error", message}}));
    res.prepare_payload();
}
