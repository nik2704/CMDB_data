#include "Server.h"


Server::Server(int port, size_t thread_count)
    : ioc_(thread_count),
      acceptor_(ioc_, {tcp::v4(), static_cast<net::ip::port_type>(port)}),
      pool_(thread_count),
      cmdb_(cmdb::CMDB::getInstance("cmdb.bin")),
      data_store_(cmdb_),
      handler_(data_store_) {}

Server::~Server() {
    cmdb_.saveToFile();
}

void Server::Run() {
    std::cout << "HTTP сервер запущен на порту " << acceptor_.local_endpoint().port() << "\n";
    AcceptConnections();
    ioc_.run();
}

void Server::AcceptConnections() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            pool_.enqueue([this, socket = std::make_shared<tcp::socket>(std::move(socket))]() mutable {
                Session(socket);
            });
        }
        AcceptConnections();
    });
}

void Server::Session(std::shared_ptr<tcp::socket> socket) {
    try {
        std::cout << "Запрос обработан в потоке: " << std::this_thread::get_id() << std::endl;
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(*socket, buffer, req);
        http::response<http::string_body> res;
        handler_.HandleRequest(req, res);
        http::write(*socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
}
