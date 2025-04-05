#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <unordered_map>
#include <thread>
#include "ThreadPool/ThreadPool.h"
#include "Model/DataStore.h"
#include "Controller/RequestHandler.h"
#include "../CMDB/CMDB.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace json = boost::json;
using tcp = net::ip::tcp;

class Server {
public:
    Server(int port, size_t thread_count, std::string db);
    ~Server();
    
    void Run();
private:
    void AcceptConnections();
    void Session(std::shared_ptr<tcp::socket> socket);
    std::string db_;
    net::io_context ioc_;
    tcp::acceptor acceptor_;
    ThreadPool pool_;
    cmdb::CMDB& cmdb_;
    DataStore data_store_;
    RequestHandler handler_;
};
