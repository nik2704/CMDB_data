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
    void HandleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);
private:
void HandleGetAll(http::response<http::string_body>& res);
    void HandleGetLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleGetCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleGetRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void HandleAddLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleAddCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleAddRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void HandleDeleteLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleDeleteCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleDeleteRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void HandleUpdateCi(http::request<http::string_body>& req, http::response<http::string_body>& res);
    void HandleUpdateLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    void HandleGetProps(http::request<http::string_body>& req, http::response<http::string_body>& res);
    bool isResultSuccess(json::object& result);

    std::map<std::string, std::string> getQueryParams(http::request<http::string_body>& req);
    DataStore& store_;

};