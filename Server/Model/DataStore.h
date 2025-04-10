/**
 * @file DataStore.h
 * @brief Интерфейс класса DataStore для управления данными в CMDB.
 */

#pragma once

#include <boost/json.hpp>
#include <string>
#include <map>
#include <unordered_map>
#include <thread>
#include "../../CMDB/CMDB.h"

namespace json = boost::json;

/**
 * @class DataStore
 * @brief Класс-адаптер, предоставляющий API для работы с объектами CMDB: уровнями, CI и связями.
 */
class DataStore {
public:
    /**
     * @brief Конструктор с внешней ссылкой на экземпляр CMDB.
     * @param cmdb Ссылка на объект CMDB.
     */
    DataStore(cmdb::CMDB& cmdb);

    /**
     * @brief Получить все данные: уровни, CI и связи.
     * @return JSON-объект с полной структурой данных.
     */
    json::object getAllRecords();

    /**
     * @brief Получить список всех уровней.
     * @return JSON-массив с уровнями.
     */
    json::array getAllLevels();

    /**
     * @brief Получить уровень по ID.
     * @param id Идентификатор уровня.
     * @return JSON-объект уровня.
     */
    json::object getLevel(int id);

    /**
     * @brief Получить список CI по заданным фильтрам.
     * @param filters Карта фильтров (ключ-значение).
     * @return JSON-массив CI.
     */
    json::array getCi(const std::map<std::string, std::string>& filters);

    /**
     * @brief Получить список связей по фильтрам.
     * @param filters Карта фильтров (ключ-значение).
     * @return JSON-массив связей.
     */
    json::array getRelationships(const std::map<std::string, std::string>& filters);

    /**
     * @brief Добавить новый уровень.
     * @param level JSON-объект уровня.
     * @return JSON-объект с результатом добавления.
     */
    json::object addLevel(const json::object& level);

    /**
     * @brief Добавить один CI.
     * @param ci JSON-объект CI.
     * @return JSON-объект с результатом.
     */
    boost::json::object addCi(const json::object& ci);

    /**
     * @brief Добавить несколько CI.
     * @param cis JSON-массив объектов CI.
     * @return JSON-объект с результатом.
     */
    boost::json::object addCis(const json::array& cis);

    /**
     * @brief Добавить одну связь в CMDB.
     * @param ci Объект связи.
     * @param message Сообщение об ошибке или успехе.
     * @return true если добавление прошло успешно.
     */
    bool addRelationshipToCMDB(const json::object& ci, std::string& message);

    /**
     * @brief Добавить одну связь.
     * @param relationships JSON-объект связи.
     * @return JSON-объект с результатом.
     */
    boost::json::object addRelationship(const boost::json::object& relationships);

    /**
     * @brief Добавить несколько связей.
     * @param relationships JSON-массив связей.
     * @return JSON-объект с результатом.
     */
    boost::json::object addRelationships(const json::array& relationships);

    /**
     * @brief Удалить уровень по ID.
     * @param id Идентификатор уровня.
     * @return JSON-объект с результатом.
     */
    json::object deleteLevel(int id);

    /**
     * @brief Удалить CI по ID.
     * @param id Идентификатор CI.
     * @return JSON-объект с результатом.
     */
    json::object deleteCi(const std::string& id);

    /**
     * @brief Удалить связи по фильтрам.
     * @param filters Карта фильтров.
     * @return JSON-объект с результатом.
     */
    boost::json::object deleteRelationships(const std::map<std::string, std::string>& filters);

    /**
     * @brief Обновить один CI.
     * @param ci JSON-объект CI.
     * @return JSON-объект с результатом.
     */
    json::object updateCi(const json::object& ci);

    /**
     * @brief Обновить несколько CI.
     * @param cis JSON-массив CI.
     * @return JSON-объект с результатом.
     */
    json::object updateCis(const json::array& cis);

    /**
     * @brief Обновить уровень.
     * @param level JSON-объект уровня.
     * @return JSON-объект с результатом.
     */
    json::object updateLevel(const json::object& level);

    /**
     * @brief Получить список всех доступных свойств CI.
     * @return JSON-объект с именами свойств.
     */
    json::object getPropsList();

private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_; ///< Внутреннее хранилище данных.
    cmdb::CMDB& cmdb_; ///< Ссылка на CMDB-объект.

    /**
     * @brief Добавить CI в CMDB.
     */
    bool addCiToCMDB(const json::object& ci, std::string& message, std::string& ciId);

    /**
     * @brief Обновить CI в CMDB.
     */
    bool updateCiInCMDB(const json::object& ci, std::string& message, std::string& ciId);

    /**
     * @brief Проверка существования CI по ID.
     */
    bool isCIexists(std::string id);
};
