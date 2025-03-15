#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "../Model/DataStore.h"

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
public:
    explicit RequestHandler(DataStore& store) : store_(store) {}
    void HandleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
private:
    void HandleGetRequest(http::response<http::string_body>& res);
    void HandlePostRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandlePutRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandlePatchRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
    DataStore& store_;
};