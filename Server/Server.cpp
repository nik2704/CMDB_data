#include "Server.h"


Server::Server(int port, size_t thread_count, std::string db)
    : db_(std::move(db)),
      ioc_(thread_count),
      acceptor_(ioc_, {tcp::v4(), static_cast<net::ip::port_type>(port)}),
      pool_(thread_count),
      cmdb_(cmdb::CMDB::getInstance(db_)),
      data_store_(cmdb_),
      handler_(data_store_) {}

Server::~Server() {
    cmdb_.saveToFile();
}

void Server::Run() {
    std::cout << "HTTP сервер запущен на порту " << acceptor_.local_endpoint().port() << "\n";
    acceptConnections();
    ioc_.run();
}

void Server::acceptConnections() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            pool_.enqueue([this, socket = std::make_shared<tcp::socket>(std::move(socket))]() mutable {
                session(socket);
            });
        }
        acceptConnections();
    });
}

void Server::session(std::shared_ptr<tcp::socket> socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(*socket, buffer, req);

        http::response<http::string_body> res;
        handler_.handleRequest(req, res);

        http::write(*socket, res);

        beast::error_code ec;
        socket->shutdown(tcp::socket::shutdown_send, ec);

        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error(ec);
        }

    } catch (const beast::system_error& e) {
        if (e.code() == http::error::end_of_stream) {
            return;
        }
        std::cerr << "Beast ошибка: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
}
