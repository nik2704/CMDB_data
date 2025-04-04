/**
 * @file CMDB.h
 * @brief Объявление класса CMDB, управляющего конфигурационными единицами и их связями.
 *
 * Этот файл содержит объявление класса `CMDB`, который предоставляет функциональность для управления
 * конфигурационными единицами (CI) и их связями, включая добавление, удаление, обновление, поиск и сохранение.
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <boost/json.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "CI.h"
#include "Relationship.h"

namespace cmdb {

/**
 * @class CMDB
 * @brief Класс, представляющий базу данных управления конфигурациями (CMDB).
 *
 * Этот класс предоставляет интерфейс для управления конфигурационными единицами (CI) и их связями,
 * включая добавление, удаление, обновление, поиск и сохранение данных.
 */
class CMDB {
public:
    /**
     * @brief Тип указателя на конфигурационную единицу.
     */
    using CIPtr = std::shared_ptr<CI>;

    /**
     * @brief Тип списка конфигурационных единиц.
     */
    using CIList = std::deque<CIPtr>;

    /**
     * @brief Тип карты идентификаторов конфигурационных единиц к указателям.
     */
    using CIMap = std::unordered_map<std::string, CIPtr>;

    /**
     * @brief Тип карты свойств конфигурационных единиц к списку указателей на КЕ, которые данные свойства содержат.
     */
    using CIPropertyMap = std::unordered_map<std::string, CIList>;

    /**
     * @brief Тип карты связей между конфигурационными единицами.
     */
    using RelationshipMap = std::unordered_multimap<std::string, Relationship>;

    /**
     * @brief Тип обратного индекса для поиска зависимых CI.
     */
    using ReverseIndex = std::unordered_map<std::string, std::unordered_set<std::string>>;

    /**
     * @brief Тип указателя на связь между конфигурационными единицами.
     */
    using RelationshipPtr = std::shared_ptr<Relationship>;

    /**
     * @brief Получить экземпляр CMDB (синглтон).
     *
     * @param filename Имя файла для сохранения и загрузки данных.
     * @return Ссылка на экземпляр CMDB.
     */
    static CMDB& getInstance(const std::string& filename);

    /**
     * @brief Запрещены копирование и перемещение.
     */
    CMDB(const CMDB&) = delete;
    CMDB& operator=(const CMDB&) = delete;
    CMDB(CMDB&&) = delete;
    CMDB& operator=(CMDB&&) = delete;

    /**
     * @brief Деструктор класса CMDB.
     */
    ~CMDB();
    
    /**
     * @brief Добавить уровень конфигурационных единиц.
     *
     * @param name Имя уровня.
     * @return Индекс добавленного уровня.
     */
    int addLevel(const std::string& name);

    /**
     * @brief Добавить несколько уровней конфигурационных единиц.
     *
     * @param new_levels Указатель на вектор имен уровней.
     */
    void addLevels(const std::vector<std::string>* new_levels);

    /**
     * @brief Установить уровни конфигурационных единиц.
     *
     * @param new_levels Указатель на вектор имен уровней.
     * @return true, если установка прошла успешно, иначе false.
     */
    bool setLevels(const std::vector<std::string>* new_levels);

    /**
     * @brief Переименовать уровень.
     *
     * @param index Индекс уровня.
     * @param new_name Новое название.
     * @return true, если переименование прошло успешно, иначе false.
     */
    bool renameLevel(size_t index, std::string new_name);

    /**
     * @brief Получить имя уровня по индексу.
     *
     * @param index Индекс уровня.
     * @return Имя уровня или std::optional с пустым значением, если индекс не найден.
     */
    std::optional<std::string> getLevelName(int index);

    /**
     * @brief Получить индекс уровня по имени.
     *
     * @param name Имя уровня.
     * @return Индекс уровня или std::optional с пустым значением, если имя не найдено.
     */
    std::optional<int> getLevelIndex(const std::string& name);

    /**
     * @brief Получить список уровней конфигурационных единиц.
     *
     * @return Указатель на вектор имен уровней.
     */
    std::shared_ptr<std::vector<std::string>> getLevels() const;

    /**
     * @brief Удалить уровень по индексу.
     *
     * Удаление уровня, если с ним не ассоциирован не один КЕ.
     * 
     * @param index Индекс уровня.
     * @return true если уровень удален.
     */
    bool removeLevel(size_t index);

    /**
     * @brief Добавить конфигурационную единицу.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param name Имя конфигурационной единицы.
     * @param type Тип конфигурационной единицы.
     * @param level Уровень конфигурационной единицы.
     * @param properties Набор свойств конфигурационной единицы (по умолчанию пустой).
     * @return true, если добавление прошло успешно, иначе false.
     */
    bool addCI(const std::string& id, const std::string& name, const std::string& type, int level, const std::unordered_map<std::string, std::string>& properties = {});

    /**
     * @brief Удалить конфигурационную единицу по идентификатору.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @return true, если удаление прошло успешно, иначе false.
     */
    bool removeCI(const std::string& id);
    
    /**
     * @brief Получить конфигурационную единицу по идентификатору.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @return Указатель на конфигурационную единицу или nullptr, если идентификатор не найден.
     */
    CIPtr getCI(const std::string& id) const;

    /**
     * @brief Получить список всех конфигурационных единиц.
     *
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CIPtr>> getCIs() const;

    /**
     * @brief Получить список конфигурационных единиц по уровню.
     *
     * @param level Уровень конфигурационных единиц.
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CIPtr>> getCIs(int level) const;

    /**
     * @brief Получить список конфигурационных единиц по уровню и типу.
     *
     * @param level Уровень конфигурационных единиц.
     * @param type Тип конфигурационных единиц.
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CIPtr>> getCIs(int level, const std::string& type) const;

    /**
     * @brief Получить список конфигурационных единиц по типу.
     *
     * @param type Тип конфигурационных единиц.
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CIPtr>> getCIs(const std::string& type) const;

    /**
     * @brief Получить список конфигурационных единиц по произвольному запросу.
     *
     * @param filters фильтры (поле - значение).
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CMDB::CIPtr>> getCIs(const std::map<std::string, std::string>& filters) const;

    /**
     * @brief Получить список конфигурационных единиц, связанных с указанной CI на заданное количество шагов.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param steps Количество шагов для поиска связанных CI.
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    std::shared_ptr<std::vector<CIPtr>> getCIs(const std::string& id, size_t steps) const;

    /**
     * @brief Обновить свойства конфигурационной единицы.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param properties Новый набор свойств конфигурационной единицы.
     * @return true, если обновление прошло успешно, иначе false.
     */
    bool updateCI(const std::string& id, const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Обновить имя, уровень и свойства конфигурационной единицы.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param name Новое имя конфигурационной единицы.
     * @param level Новый уровень конфигурационной единицы.
     * @param properties Новый набор свойств конфигурационной единицы.
     * @return true, если обновление прошло успешно, иначе false.
     */
    bool updateCI(const std::string& id, const std::string& name, int level, const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Обновить свойства конфигурационной единицы.
     *
     * @param ci JSON объект конфигурационной единицы.
     * @param message Результат обновления.
     * @return true, если обновление прошло успешно, иначе false.
     */
    bool updateCI (cmdb::CMDB::CIPtr current_ci, const boost::json::object &ci, std::string &message);

    /**
     * @brief Установить свойство конфигурационной единицы.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @param property_name Имя свойства.
     * @param property_value Значение свойства.
     * @return true, если установка прошла успешно, иначе false.
     */
    bool setProperty(const std::string& id, const std::string& property_name, const std::string& property_value);

    /**
     * @brief Добавить связь между конфигурационными единицами.
     *
     * @param from_id Идентификатор исходной конфигурационной единицы.
     * @param to_id Идентификатор целевой конфигурационной единицы.
     * @param type Тип связи.
     * @return true, если добавление прошло успешно, иначе false.
     */
    bool addRelationship(const std::string& from_id, const std::string& to_id, const std::string& type);

    /**
     * @brief Удалить связь между конфигурационными единицами.
     *
     * @param from_id Идентификатор исходной конфигурационной единицы.
     * @param to_id Идентификатор целевой конфигурационной единицы.
     * @return true, если удаление прошло успешно, иначе false.
     */
    bool removeRelationship(const std::string& from_id, const std::string& to_id);

    /**
     * @brief Удалить все связи, связанные с указанной CI.
     *
     * @param id Идентификатор конфигурационной единицы.
     */
    void removeRelationshipsForId(const std::string& id);

    /**
     * @brief Получить список всех связей.
     *
     * @return Указатель на вектор указателей на связи.
     */
    std::shared_ptr<std::vector<CMDB::RelationshipPtr>> getRelationships() const;

    /**
     * @brief Получить список связей, исходящих из указанной CI.
     *
     * @param from_id Идентификатор исходной конфигурационной единицы.
     * @return Указатель на вектор указателей на связи.
     */
    std::shared_ptr<std::vector<CMDB::RelationshipPtr>> getRelationships(const std::string& from_id) const;

    /**
     * @brief Получить список связей между двумя указанными CI.
     *
     * @param from_id Идентификатор исходной конфигурационной единицы.
     * @param to_id Идентификатор целевой конфигурационной единицы.
     * @return Указатель на вектор указателей на связи.
     */
    std::shared_ptr<std::vector<CMDB::RelationshipPtr>> getRelationships(const std::string& from_id, const std::string& to_id) const;

    /**
     * @brief Получить список связей между двумя указанными CI указанного типа.
     *
     * @param from_id Идентификатор исходной конфигурационной единицы.
     * @param to_id Идентификатор целевой конфигурационной единицы.
     * @param type Тип связи.
     * @return Указатель на вектор указателей на связи.
     */
    std::shared_ptr<std::vector<CMDB::RelationshipPtr>> getRelationships(const std::string& from_id, const std::string& to_id, const std::string& type) const;

    /**
     * @brief Получить список зависимых конфигурационных единиц от указанной CI.
     *
     * @param id Идентификатор конфигурационной единицы.
     * @return Указатель на вектор указателей на связи.
     */
    std::shared_ptr<std::vector<CMDB::RelationshipPtr>> getDependentCIs(const std::string& id) const;

    /**
     * @brief Преобразовать вектор объектов в JSON-объект.
     *
     * @tparam T Тип объектов в векторе.
     * @param items Указатель на вектор объектов.
     * @param key Ключ для JSON-объекта.
     * @return JSON-объект, содержащий вектор объектов.
     */
    template <typename T>
    static boost::json::object toJSON(std::shared_ptr<std::vector<T>> items, const std::string& key);

    /**
     * @brief Сохранить данные CMDB в файл.
     *
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool saveToFile();

    /**
     * @brief Необходимость сохрания данных CMDB в файл.
     */
    void markAsModified();

    /**
     * @brief Сохранить данные CMDB в указанный файл.
     *
     * @param filename Имя файла для сохранения.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool saveToFile(const std::string& filename);

    /**
     * @brief Запустить автоматическое сохранение данных CMDB в файл.
     */
    void startAutoSave();

    /**
     * @brief Загрузить данные CMDB из файла.
     *
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool loadFromFile();

    /**
     * @brief Загрузить данные CMDB из указанного файла.
     *
     * @param filename Имя файла для загрузки.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Получение списка свойств.
     */
    boost::json::array getProps() const;

private:
    std::string filename_; ///< Имя файла для сохранения и загрузки данных.
    CIList all_cis_; ///< Список всех конфигурационных единиц.
    CIMap id_to_ci_; ///< Карта идентификаторов конфигурационных единиц к указателям.
    CIPropertyMap property_to_cis_; ///< Карта свойств конфигурационных единиц к списку указателей на КЕ.
    std::vector<std::string> levels_; ///< Список уровней конфигурационных единиц.
    RelationshipMap relationships_; ///< Карта связей между конфигурационными единицами.
    ReverseIndex reverse_index_; ///< Обратный индекс для поиска зависимых CI.

    mutable std::mutex cis_mutex_; ///< Мьютекс для защиты доступа к конфигурационным единицам.
    mutable std::mutex dependencies_mutex_; ///< Мьютекс для защиты доступа к связям.

    std::atomic<bool> saving_{false}; ///< Флаг, указывающий, выполняется ли сохранение.
    std::atomic<bool> stop_thread_{false}; ///< Флаг, указывающий, нужно ли остановить поток автоматического сохранения.
    std::thread auto_save_thread_; ///< Поток для автоматического сохранения данных.
    std::condition_variable stop_condition_; ///< Условная переменная для прерывания потока автоматического сохранения.
    std::mutex stop_mutex_; ///< Мьютекс для защиты условной переменной и флага прерывания.

    bool modified_ = false; ///< Флаг, указывающий, были ли внесены изменения в данные.
    static std::unique_ptr<CMDB> instance_; ///< Уникальный указатель на экземпляр CMDB (синглтон).
    static std::once_flag init_flag_; ///< Флаг для инициализации синглтона.
    CMDB() = default; ///< Конструктор по умолчанию (приватный для синглтона).

    /**
     * @brief Внутренняя функция для получения списка конфигурационных единиц с использованием предиката.
     *
     * @tparam Predicate Тип предиката.
     * @param pred Предикат для фильтрации конфигурационных единиц.
     * @return Указатель на вектор указателей на конфигурационные единицы.
     */
    template <typename Predicate>
    std::shared_ptr<std::vector<CIPtr>> getCIsImpl(Predicate pred) const;

    /**
     * @brief Внутренняя функция для получения списка связей с использованием предиката.
     *
     * @tparam Predicate Тип предиката.
     * @param pred Предикат для фильтрации связей.
     * @return Указатель на вектор указателей на связи.
     */
    template <typename Predicate>
    std::shared_ptr<std::vector<RelationshipPtr>> getRelationshipsImpl(Predicate pred) const;

    /**
     * @brief Сохранить коллекцию строк в файл.
     *
     * @param out Поток вывода для сохранения.
     * @param collection Коллекция строк.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool saveCollection(std::ofstream& out, const std::vector<std::string>& collection);

    /**
     * @brief Сохранить коллекцию конфигурационных единиц в файл.
     *
     * @param out Поток вывода для сохранения.
     * @param collection Коллекция конфигурационных единиц.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool saveCollection(std::ofstream& out, const CIList& collection);

    /**
     * @brief Сохранить коллекцию связей в файл.
     *
     * @param out Поток вывода для сохранения.
     * @param collection Коллекция связей.
     * @return true, если сохранение прошло успешно, иначе false.
     */
    bool saveCollection(std::ofstream& out, const RelationshipMap& collection);

    /**
     * @brief Загрузить коллекцию строк из файла.
     *
     * @param in Поток ввода для загрузки.
     * @param collection Коллекция строк.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool loadCollection(std::ifstream& in, std::vector<std::string>& collection);

    /**
     * @brief Загрузить коллекцию конфигурационных единиц из файла.
     *
     * @param in Поток ввода для загрузки.
     * @param collection Коллекция конфигурационных единиц.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool loadCollection(std::ifstream& in, CIList& collection);

    /**
     * @brief Загрузить коллекцию связей из файла.
     *
     * @param in Поток ввода для загрузки.
     * @param collection Коллекция связей.
     * @return true, если загрузка прошла успешно, иначе false.
     */
    bool loadCollection(std::ifstream& in, RelationshipMap& collection);

    /**
     * @brief Восстановить карту идентификаторов конфигурационных единиц к указателям.
     */
    void restoreIDtoCI();

    /**
     * @brief Восстановить обратный индекс для поиска зависимых CI.
     */
    void restoreReverseIndex();

    /**
     * @brief Восстановить карту свойств и CI.
     */
    void restorePropertiesMap();

    /**
     * @brief Цикл автоматического сохранения данных CMDB.
     */
    void autoSaveLoop();

    /**
     * @brief Обновление карты свойств.
     */
    void updatePropertiesMap(CIPtr ci, const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Удаление свойства из карты свойств.
     */
    void deletePropertyFromMap(CIPtr ci, const std::string& property_name);

    void deletePropertiesMap(CIPtr ci, const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief Создание или дополнение свойства в карты свойств.
     */
    void addPropertyToMap(CIPtr ci, const std::string& property_name, const std::string& property_value);
};

/**
 * @brief Преобразовать вектор объектов в JSON-объект.
 *
 * @tparam T Тип объектов в векторе.
 * @param items Указатель на вектор объектов.
 * @param key Ключ для JSON-объекта.
 * @return JSON-объект, содержащий вектор объектов.
 */
template <typename T>
boost::json::object CMDB::toJSON(std::shared_ptr<std::vector<T>> items, const std::string& key) {
    boost::json::array json_array;

    if (!items) {
        return boost::json::object();
    }

    if (items->empty()) {
        std::cerr << "Warning: items vector is empty.\n";
    }

    for (const auto& item : *items) {
        if constexpr (std::is_same_v<T, std::string>) {
            json_array.push_back(boost::json::string(item));
        } else {
            if (item) {
                json_array.push_back(item->asJSON());
            } else {
                json_array.push_back(boost::json::object());
            }
        }
    }

    boost::json::object result;
    result[key] = std::move(json_array);
    return result;
}

} // namespace cmdb