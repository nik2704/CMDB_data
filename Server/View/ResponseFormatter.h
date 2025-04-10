/**
 * @file ResponseFormatter.h
 * @brief Заголовочный файл с утилитами для форматирования HTTP-ответов в JSON-формате.
 */

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

/**
 * @class ResponseFormatter
 * @brief Статический класс для форматирования HTTP-ответов с использованием JSON.
  */
class ResponseFormatter {
public:
    /**
     * @brief Формирует JSON-ответ на основе объекта JSON.
     * @param res Ссылка на HTTP-ответ, который будет заполнен.
     * @param obj JSON-объект, который будет сериализован в тело ответа.
     */
    static void makeJSONResponse(http::response<http::string_body>& res, const json::object& obj);

    /**
     * @brief Формирует JSON-ответ на основе массива JSON.
     * @param res Ссылка на HTTP-ответ, который будет заполнен.
     * @param arr JSON-массив, который будет сериализован в тело ответа.
     */
    static void makeJSONResponse(http::response<http::string_body>& res, const json::array& arr);

    /**
     * @brief Формирует ответ с ошибкой.
     * @param res Ссылка на HTTP-ответ, который будет заполнен.
     * @param status HTTP-статус ошибки.
     * @param message Сообщение об ошибке, которое будет включено в тело ответа.
     */
    static void makeErrorResponse(http::response<http::string_body>& res, http::status status, const std::string& message);
};
