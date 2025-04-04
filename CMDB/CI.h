/**
 * @file CI.h
 * @brief Объявление класса Конфигурационная единица (CI).
 *
 * Этот файл содержит объявление класса `CI`, который представляет собой конфигурационную единицу
 * с идентификатором, именем, типом, уровнем и набором свойств.
 */

#pragma once

#include <boost/json.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

namespace cmdb {

/**
 * @class CI
 * @brief Класс, представляющий конфигурационную единицу (CI).
 */
class CI {
public:
    /**
     * @brief Конструктор по умолчанию.
     */
    CI() : id_(""), name_(""), type_(""), level_(0), properties_() {}

    /**
     * @brief Конструктор с параметрами.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param name Имя конфигурационной единицы.
     * @param type Тип конфигурационной единицы.
     */
    CI(const std::string& id, const std::string& name, const std::string& type);

    /**
     * @brief Конструктор с параметрами, включая уровень и свойства.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param name Имя конфигурационной единицы.
     * @param type Тип конфигурационной единицы.
     * @param level Уровень конфигурационной единицы.
     * @param properties Набор свойств конфигурационной единицы (ключ-значение).
     */
    CI(const std::string& id, const std::string& name, const std::string& type,
        int level, const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Получить идентификатор конфигурационной единицы.
     *
     * @return Идентификатор конфигурационной единицы.
     */
    std::string getId() const;

    /**
     * @brief Получить имя конфигурационной единицы.
     *
     * @return Имя конфигурационной единицы.
     */
    std::string getName() const;

    /**
     * @brief Получить тип конфигурационной единицы.
     *
     * @return Тип конфигурационной единицы.
     */
    std::string getType() const;

    /**
     * @brief Получить уровень конфигурационной единицы.
     *
     * @return Уровень конфигурационной единицы.
     */
    int getLevel() const;

    /**
     * @brief Получить набор свойств конфигурационной единицы.
     *
     * @return Константная ссылка на набор свойств конфигурационной единицы.
     */
    const std::unordered_map<std::string, std::string>& getProperties() const;

    /**
     * @brief Получить конфигурационную единицу в виде JSON-строки.
     *
     * @return JSON-представление конфигурационной единицы в виде строки.
     */
    std::string getCIasJSONstring() const;

    /**
     * @brief Получить конфигурационную единицу в виде объекта JSON.
     *
     * @return JSON-объект, представляющий конфигурационную единицу.
     */
    boost::json::object asJSON() const;

    /**
     * @brief Установить имя конфигурационной единицы.
     *
     * @param name Новое имя конфигурационной единицы.
     * @return true если изменено
     */
    bool setName(const std::string& name);

    /**
     * @brief Установить уровень конфигурационной единицы.
     *
     * @param level Новый уровень конфигурационной единицы.
     * @return true если изменено
     */
    bool setLevel(int level);

    /**
     * @brief Установить свойство конфигурационной единицы.
     *
     * @param key Свойство.
     * @param value Значение свойства.
     * @return true если изменено
     */
    bool setProperty(const std::string& key, const std::optional<std::string>& value);

    /**
     * @brief Установить набор свойств конфигурационной единицы.
     *
     * @param properties Новый набор свойств конфигурационной единицы.
     */
    void setProperties(const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Установить набор свойств конфигурационной единицы по объекту JSON.
     *
     * @param update_ci JSON обновления конфигурационной единицы.
     * @param message возврат сообщения
     * @return true если что-то изменено
     */
    bool setProperties(const boost::json::object& update_ci, std::string& message);

    /**
     * @brief Проверить, существует ли свойство с указанным ключом.
     *
     * @param key Свойство.
     * @return true, если свойство существует, иначе false.
     */
    bool hasProperty(const std::string& key) const;

    /**
     * @brief Получить значение свойства по ключу.
     *
     * @param key Свойство.
     * @return Значение свойства, если оно существует, иначе std::optional с пустым значением.
     */
    std::optional<std::string> getProperty(const std::string& key) const;

    /**
     * @brief Удалить свойство по ключу.
     *
     * @param key Свойство.
     * @return true, если свойство было удалено, иначе false.
     */
    bool removeProperty(const std::string& key);

    /**
     * @brief Сохранить конфигурационную единицу в файл.
     *
     * @param out Поток вывода для сохранения.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool save(std::ofstream& out) const;

    /**
     * @brief Загрузить конфигурационную единицу из файла.
     *
     * @param in Поток ввода для загрузки.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool load(std::ifstream& in);

    /**
     * @brief Вывести информацию о конфигурационной единице в консоль.
     */
    void print() const;

private:
    std::string id_; ///< Идентификатор конфигурационной единицы.
    std::string name_; ///< Имя конфигурационной единицы.
    std::string type_; ///< Тип конфигурационной единицы.
    int level_; ///< Уровень конфигурационной единицы.
    std::unordered_map<std::string, std::string> properties_; ///< Набор свойств конфигурационной единицы.
};

} // namespace cmdb