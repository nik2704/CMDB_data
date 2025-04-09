#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <sstream>
#include <map>
#include "../Model/DataStore.h"

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
public:
    explicit RequestHandler(DataStore& store) : store_(store) {}
    void handleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
private:
    void handleGetAll(http::response<http::string_body>& res);
    void handleGetLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleGetCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleGetRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void handleAddLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleAddCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleAddRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void handleDeleteLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleDeleteCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleDeleteRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void handleUpdateCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handleUpdateLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void handleGetProps(http::request<http::string_body>& req, http::response<http::string_body>& res);
    bool isResultSuccess(json::object& result);

    std::map<std::string, std::string> getQueryParams(http::request<http::string_body>& req);
    DataStore& store_;

};