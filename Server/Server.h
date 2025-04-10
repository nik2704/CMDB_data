/**
 * @file Server.h
 * @brief Заголовочный файл класса Server, реализующего многопоточный HTTP-сервер с использованием Boost.Beast и Boost.Asio.
 */

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

/**
 * @class Server
 * @brief Класс, реализующий асинхронный HTTP-сервер с обработкой запросов в пуле потоков.
 *
 * Сервер принимает входящие соединения, распределяет задачи по потокам и обрабатывает HTTP-запросы,
 * используя RequestHandler и доступ к данным через DataStore и CMDB.
 */
class Server {
public:
    /**
     * @brief Конструктор сервера.
     * @param port Порт для прослушивания входящих соединений.
     * @param thread_count Количество рабочих потоков.
     * @param db Путь или идентификатор базы данных.
     */
    Server(int port, size_t thread_count, std::string db);

    /**
     * @brief Деструктор сервера.
     */
    ~Server();

    /**
     * @brief Запускает сервер: инициализирует прослушивание и потоковый пул.
     */
    void Run();

private:
    /**
     * @brief Принимает входящие соединения и запускает сессию обработки запроса.
     */
    void acceptConnections();

    /**
     * @brief Обрабатывает отдельную сессию HTTP-запроса.
     * @param socket Указатель на сокет подключения.
     */
    void session(std::shared_ptr<tcp::socket> socket);

    std::string db_;                      ///< Имя или путь к базе данных.
    net::io_context ioc_;                ///< Контекст ввода/вывода Boost.Asio.
    tcp::acceptor acceptor_;             ///< Acceptor для входящих соединений.
    ThreadPool pool_;                    ///< Пул потоков для обработки запросов.
    cmdb::CMDB& cmdb_;                   ///< Ссылка на объект CMDB.
    DataStore data_store_;              ///< Объект хранилища данных.
    RequestHandler handler_;            ///< Объект обработчика HTTP-запросов.
};
