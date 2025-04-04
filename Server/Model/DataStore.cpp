#include "DataStore.h"

DataStore::DataStore(cmdb::CMDB& cmdb) : cmdb_(cmdb) {}

    json::object DataStore::GetAllRecords() {
        json::object result;

        if (auto levels = cmdb_.getLevels()) {
            json::array levelsArray;

            for (const auto& level : *levels) {
                levelsArray.push_back(json::value(level));
            }

            result["levels"] = levelsArray;
        }

        if (auto cis = cmdb_.getCIs()) {
            json::array cisArray;
            for (const auto& ciPtr : *cis) {
                if (ciPtr) {
                    json::object ciObject;
                    ciObject = ciPtr->asJSON();
                    cisArray.push_back(ciObject);
                }
            }
            result["cis"] = cisArray;
        }

        if (auto relationships = cmdb_.getRelationships()) {
            json::array relationshipsArray;
            for (const auto& relationshipPtr : *relationships) {
                if (relationshipPtr) {
                    json::object relationshipObject;
                    relationshipObject["from_id"] = relationshipPtr->getSource();
                    relationshipObject["to_id"] = relationshipPtr->getDestination();
                    relationshipObject["type"] = relationshipPtr->getType();
                    relationshipsArray.push_back(relationshipObject);
                }
            }
            result["relationships"] = relationshipsArray;
        }

        return result;
    }

    json::array DataStore::GetAllLevels() {
        json::array levelsArray;

        if (auto levels = cmdb_.getLevels()) {
            for (const auto& level : *levels) {
                levelsArray.push_back(json::value(level));
            }
        }

        return levelsArray;
    }

    json::object DataStore::GetLevel(int id) {
        json::object result;

        auto levelName = cmdb_.getLevelName(id);

        if (levelName.has_value()) result["name"] = levelName.value();

        return result;
    }

    json::array DataStore::GetCi(const std::map<std::string, std::string>& filters) {
        std::shared_ptr<std::vector<cmdb::CMDB::CIPtr>> cis = cmdb_.getCIs(filters);

        json::array result;
        for (const auto& ci : *cis) {
            json::object ciObject;
            ciObject = ci->asJSON();
            result.push_back(ciObject);
        }

        return result;
    }

    json::array DataStore::GetRelationships(const std::map<std::string, std::string>& filters) {
        // Заглушка: возвращает пустой JSON-массив
        return json::array();
    }

    json::object DataStore::AddLevel(const json::object& level) {
        json::object result;

        if (!level.contains("name")) {
            result["status"] = "failure";
            result["error"] = "оОтсутствует тег name";
        }

        std::string name = boost::json::value_to<std::string>(level.at("name"));
        int levelNum = cmdb_.addLevel(name);

        result["id"] = levelNum;
        result["name"] = name;

        return result;
    }

    bool DataStore::AddCiToCMDB(const json::object& ci, std::string& message, std::string& ciId) {
        if (!ci.contains("id") || !ci.contains("name") || !ci.contains("type") || !ci.contains("level")) {
            message = "Не запонены обязательные поля.";
            return false;
        }

        ciId = boost::json::value_to<std::string>(ci.at("id"));
        std::string name = boost::json::value_to<std::string>(ci.at("name"));
        std::string ciType = boost::json::value_to<std::string>(ci.at("type"));
        int level = boost::json::value_to<int>(ci.at("level"));

        if (isCIexists(ciId)) {
            message = "КЕ с таким id " + ciId + " уже существует.";
            return false;
        }

        std::unordered_map<std::string, std::string> properties;
        if (ci.contains("properties")) {
            if (!ci.at("properties").is_object()) {
                message = "Data Store (AddCi): Свойства должны быть объектом JSON.";
                return false;
            }

            auto propertiesJson = ci.at("properties").as_object();

            for (auto it = propertiesJson.begin(); it != propertiesJson.end(); ++it) {
                if (!it->value().is_string()) {
                    message = "Значение свойства должны быть строковыми.";
                    return false;
                }

                properties[it->key()] = boost::json::value_to<std::string>(it->value());
            }
        }

        if (!cmdb_.addCI(ciId, name, ciType, level, properties)) {
            message = "Ошибка добавления КЕ в CMDB.";
            return false;
        }

        message = ciId + " добавлен";

        return true;
    }

    boost::json::object DataStore::AddCi(const boost::json::object& ci) {
        boost::json::object result = ci;
        result["status"] = "success";
        result["id"] = "unknown";

        std::string message;
        std::string id;

        if (!AddCiToCMDB(ci, message, id)) {
            result["status"] = "failure";
        }

        result["message"] = message;
        result["id"] = id;

        return result;
    }

    boost::json::object DataStore::AddCis(const boost::json::array& cis) {
        boost::json::object result;
        boost::json::array cis_add;
        int addedCount = 0;

        for (const auto& ciValue : cis) {
            boost::json::object entry;

            if (!ciValue.is_object()) {
                entry["id"] = nullptr;
                entry["message"] = "Элемент не является объектом JSON.";
                
                cis_add.push_back(entry);
                continue;
            }

            const boost::json::object& ci = ciValue.as_object();
            std::string message;
            std::string ciId = "unknown";

            if (AddCiToCMDB(ci, message, ciId)) {
                ++addedCount;
            }

            entry["id"] = ciId;
            entry["message"] = message;

            cis_add.push_back(entry);
        }

        result["cis_add"] = cis_add;

        boost::json::object info;
        info["total"] = static_cast<int>(cis.size());
        info["added"] = addedCount;
        result["info"] = info;

        result["status"] = addedCount > 0 ? "success" : "failure";

        return result;
    }


    void DataStore::AddRelationships(const json::array& relationships) {
        // Заглушка: ничего не делает
    }

    json::object DataStore::DeleteLevel(int id) {
        json::object result;

        result["status"] = "success";
        result["message"] = "Удален";

        if (!cmdb_.removeLevel(id)) {
            result["status"] = "failure";
            result["error"] = "Ошибка удаления уровня: " + id;
        }

        return result;
    }

    void DataStore::DeleteCi(const std::map<std::string, std::string>& filters) {
        // Заглушка: ничего не делает
    }

    void DataStore::DeleteRelationships(const std::map<std::string, std::string>& filters) {
        // Заглушка: ничего не делает
    }

    void DataStore::UpdateCi(const json::object& ci) {
        // Заглушка: ничего не делает
    }

    void DataStore::UpdateCis(const json::array& cis) {
        // Заглушка: ничего не делает
    }

    json::object DataStore::UpdateLevel(const json::object& level) {
        json::object result;
        result["status"] = "failure";

        if (!level.contains("name") || !level.contains("id")) {
            result["error"] = "оОтсутствует тег name или id";

            return result;
        }

        size_t id = boost::json::value_to<int>(level.at("id"));
        std::string name = boost::json::value_to<std::string>(level.at("name"));

        if (cmdb_.renameLevel(id, name)) {            
            result["status"] = "success";
        }

        json::object updt_level;
        updt_level["id"] = id;
        updt_level["name"] = name;
        result["level"] = updt_level;

        return result;    }

    int DataStore::AddRecord(const json::object& record) {
        // Заглушка: возвращает 0
        return 0;
    }

    bool DataStore::UpdateRecord(int id, const json::object& record) {
        // Заглушка: возвращает false
        return false;
    }

    bool DataStore::isCIexists(std::string id) {
        return cmdb_.getCI(id) != nullptr ? true : false;
    }
// int DataStore::AddRecord(const json::object& record) {
//     int new_id = data_.size() + 1;
//     for (auto& [key, value] : record) {
//         data_[new_id][key] = value.as_string();
//     }
//     return new_id;
// }

// bool DataStore::UpdateRecord(int id, const json::object& record) {
//     if (data_.find(id) == data_.end()) return false;
//     for (auto& [key, value] : record) {
//         data_[id][key] = value.as_sci.contains("id")
//     for (const auto& [id, record] : data_) {
//         json::object record_json;
//         for (const auto& [key, value] : record) {
//             record_json[key] = value;
//         }
//         result[std::to_string(id)] = record_json;
//     }
//     return result;
// }
