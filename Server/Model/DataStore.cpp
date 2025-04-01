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
        // Заглушка: возвращает пустой JSON-массив
        return json::array();
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

    void DataStore::AddCi(const json::object& ci) {
        // Заглушка: ничего не делает
    }

    void DataStore::AddCis(const json::array& cis) {
        // Заглушка: ничего не делает
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
