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
                    ciObject["id"] = ciPtr->getId();
                    ciObject["name"] = ciPtr->getName();
                    ciObject["type"] = ciPtr->getType();
                    ciObject["level"] = ciPtr->getLevel();
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
        // Заглушка: возвращает пустой JSON-массив
        return json::array();
    }

    json::array DataStore::GetRelationships(const std::map<std::string, std::string>& filters) {
        // Заглушка: возвращает пустой JSON-массив
        return json::array();
    }

    void DataStore::AddLevel(const json::object& level) {
        // Заглушка: ничего не делает
    }

    boost::json::object DataStore::AddCi(const boost::json::object& ci) {
        boost::json::object result = ci;
        result["status"] = "failure";

        try {
            if (!ci.contains("id") || !ci.contains("name") || !ci.contains("type") || !ci.contains("level")) {
                throw std::runtime_error("Missing required fields in CI object.");
            }

            std::string id = boost::json::value_to<std::string>(ci.at("id"));
            std::string name = boost::json::value_to<std::string>(ci.at("name"));
            std::string ciType = boost::json::value_to<std::string>(ci.at("type"));
            int level = boost::json::value_to<int>(ci.at("level"));

            if (isCIexists(id)) {
                throw std::runtime_error("CI with id " + id + " already exists.");
            }

            std::unordered_map<std::string, std::string> properties;
            if (ci.contains("properties")) {
                if (!ci.at("properties").is_object()) {
                    throw std::runtime_error("Data Store (AddCi): Properties field must be a JSON object.");
                }

                auto propertiesJson = ci.at("properties").as_object();

                for (auto it = propertiesJson.begin(); it != propertiesJson.end(); ++it) {
                    if (!it->value().is_string()) {
                        throw std::runtime_error("Properties values must be strings.");
                    }
                    properties[it->key()] = boost::json::value_to<std::string>(it->value());
                }
            }

            if (!cmdb_.addCI(id, name, ciType, level, properties)) {
                throw std::runtime_error("Failed to add CI to CMDB.");
            }

            result["status"] = "success";
        } catch (const std::exception& e) {
            result["error"] = e.what();
            std::cerr << "Error adding CI: " << e.what() << std::endl;
        }

        return result;
    }

     boost::json::object DataStore::AddCis(const json::array& cis) {
         boost::json::object result;
        // Заглушка: ничего не делает
        return result;
    }

    void DataStore::AddRelationships(const json::array& relationships) {
        // Заглушка: ничего не делает
    }

    void DataStore::DeleteLevel(int id) {
        // Заглушка: ничего не делает
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

    void DataStore::UpdateLevel(const json::object& level) {
        // Заглушка: ничего не делает
    }

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
//         data_[id][key] = value.as_string();
//     }
//     return true;
// }

// json::object DataStore::GetAllRecords() const {
//     json::object result;
//     for (const auto& [id, record] : data_) {
//         json::object record_json;
//         for (const auto& [key, value] : record) {
//             record_json[key] = value;
//         }
//         result[std::to_string(id)] = record_json;
//     }
//     return result;
// }
