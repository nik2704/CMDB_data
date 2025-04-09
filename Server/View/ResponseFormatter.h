#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;


class ResponseFormatter {
public:
    static void makeJSONResponse(http::response<http::string_body>& res, const json::object& obj);
    static void makeJSONResponse(http::response<http::string_body>& res, const json::array& arr);
    static void makeErrorResponse(http::response<http::string_body>& res, http::status status, const std::string& message);
};