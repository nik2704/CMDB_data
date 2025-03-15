#include "ResponseFormatter.h"

void ResponseFormatter::MakeJSONResponse(http::response<http::string_body>& res, const json::object& data) {
    res.result(http::status::ok);
    res.body() = json::serialize(data);
    res.prepare_payload();
}

void ResponseFormatter::MakeErrorResponse(http::response<http::string_body>& res, http::status status, const std::string& message) {
    res.result(status);
    res.body() = R"({"error": ")" + message + R"("})";
    res.prepare_payload();
}