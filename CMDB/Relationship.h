/**
 * @file Relationship.h
 * @brief Объявление класса Relationship, представляющего связь между конфигурационными единицами.
 *
 * Этот файл содержит объявление класса `Relationship`, который используется для представления
 * связей между конфигурационными единицами (CI) в CMDB.
 */

#pragma once

#include <boost/json.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace cmdb {

/**
 * @class Relationship
 * @brief Класс, представляющий связь между конфигурационными единицами (CI).
 *
 * Этот класс позволяет хранить информацию о связи между двумя CI, включая источник, назначение,
 * тип связи и вес.
 */
class Relationship {
public:
    /**
     * @brief Конструктор по умолчанию.
     */
    Relationship() = default;

    /**
     * @brief Конструктор с параметрами.
     *
     * @param source Идентификатор исходной конфигурационной единицы.
     * @param destination Идентификатор целевой конфигурационной единицы.
     * @param type Тип связи между CI.
     * @param weight Вес связи (по умолчанию 1.0).
     */
    Relationship(const std::string& source, const std::string& destination, const std::string& type, double weight = 1.0);

    /**
     * @brief Конструктор копирования.
     *
     * @param other Объект Relationship для копирования.
     */
    Relationship(const Relationship& other) = default;

    /**
     * @brief Конструктор перемещения.
     *
     * @param other Объект Relationship для перемещения.
     */
    Relationship(Relationship&& other) noexcept = default;

    /**
     * @brief Получить связь в виде JSON-строки.
     *
     * @return JSON-представление связи в виде строки.
     */
    std::string getCIasJSONstring() const;

    /**
     * @brief Получить связь в виде объекта JSON.
     *
     * @return JSON-объект, представляющий связь.
     */
    boost::json::object asJSON() const;

    /**
     * @brief Сохранить связь в файл.
     *
     * @param out Поток вывода для сохранения.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool save(std::ofstream& out) const;

    /**
     * @brief Загрузить связь из файла.
     *
     * @param in Поток ввода для загрузки.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool load(std::ifstream& in);

    /**
     * @brief Получить тип связи.
     *
     * @return Тип связи.
     */
    std::string getType() const;

    /**
     * @brief Получить идентификатор исходной конфигурационной единицы.
     *
     * @return Идентификатор исходной CI.
     */
    std::string getSource() const;

    /**
     * @brief Получить идентификатор целевой конфигурационной единицы.
     *
     * @return Идентификатор целевой CI.
     */
    std::string getDestination() const;

    /**
     * @brief Получить вес связи.
     *
     * @return Вес связи.
     */
    double getWeight() const;

private:
    std::string source_; ///< Идентификатор исходной конфигурационной единицы.
    std::string destination_; ///< Идентификатор целевой конфигурационной единицы.
    std::string type_; ///< Тип связи.
    double weight_; ///< Вес связи.
};

} // namespace cmdb