/**
 * @file RequestHandler.h
 * @brief Заголовочный файл класса RequestHandler, реализующего обработку HTTP-запросов.
 */

#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <sstream>
#include <map>
#include "../Model/DataStore.h"

namespace beast = boost::beast;
namespace http = beast::http;

/**
 * @class RequestHandler
 * @brief Класс, обрабатывающий входящие HTTP-запросы и взаимодействующий с хранилищем данных (DataStore).
 */
class RequestHandler {
public:
    /**
     * @brief Конструктор.
     * @param store Ссылка на объект DataStore для доступа к данным.
     */
    explicit RequestHandler(DataStore& store) : store_(store) {}

    /**
     * @brief Основной метод обработки запроса.
     * @param req HTTP-запрос.
     * @param res HTTP-ответ.
     */
    void handleRequest(http::request<http::string_body>& req, http::response<http::string_body>& res);

private:
    /**
     * @brief Обработка запроса получения всех данных.
     */
    void handleGetAll(http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса получения уровня по ID.
     */
    void handleGetLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса получения CI с фильтрацией.
     */
    void handleGetCi(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса получения связей.
     */
    void handleGetRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на добавление уровня.
     */
    void handleAddLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на добавление CI.
     */
    void handleAddCi(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на добавление связей.
     */
    void handleAddRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на удаление уровня.
     */
    void handleDeleteLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на удаление CI.
     */
    void handleDeleteCi(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на удаление связей.
     */
    void handleDeleteRelationships(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на обновление CI.
     */
    void handleUpdateCi(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на обновление уровня.
     */
    void handleUpdateLevel(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Обработка запроса на получение списка свойств CI.
     */
    void handleGetProps(http::request<http::string_body>& req, http::response<http::string_body>& res);

    /**
     * @brief Проверка успешности результата.
     * @param result JSON-объект с результатом.
     * @return true, если результат успешный.
     */
    bool isResultSuccess(json::object& result);

    /**
     * @brief Извлечение параметров запроса из URI.
     * @param req HTTP-запрос.
     * @return Карта параметров запроса (query string).
     */
    std::map<std::string, std::string> getQueryParams(http::request<http::string_body>& req);

    DataStore& store_; ///< Ссылка на объект хранилища данных.
};
