#include "DataStore.h"

DataStore::DataStore(cmdb::CMDB& cmdb) : cmdb_(cmdb) {}

    json::object DataStore::getAllRecords() {
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

    json::array DataStore::getAllLevels() {
        json::array levelsArray;

        if (auto levels = cmdb_.getLevels()) {
            for (const auto& level : *levels) {
                levelsArray.push_back(json::value(level));
            }
        }

        return levelsArray;
    }

    json::object DataStore::getLevel(int id) {
        json::object result;

        auto levelName = cmdb_.getLevelName(id);

        if (levelName.has_value()) result["name"] = levelName.value();

        return result;
    }

    json::array DataStore::getCi(const std::map<std::string, std::string>& filters) {
        std::shared_ptr<std::vector<cmdb::CMDB::CIPtr>> cis = cmdb_.getCIs(filters);

        json::array result;
        for (const auto& ci : *cis) {
            json::object ciObject;
            ciObject = ci->asJSON();
            result.push_back(ciObject);
        }

        return result;
    }

    json::array DataStore::getRelationships(const std::map<std::string, std::string>& filters) {
        std::shared_ptr<std::vector<cmdb::CMDB::RelationshipPtr>> rels = cmdb_.getRelationships(filters);

        json::array result;
        for (const auto& rel : *rels) {
            
            json::object ciObject;
            ciObject = rel->asJSON();
            result.push_back(ciObject);
        }

        return result;
    }

    json::object DataStore::addLevel(const json::object& level) {
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

    bool DataStore::addCiToCMDB(const json::object& ci, std::string& message, std::string& ciId) {
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

    boost::json::object DataStore::addCi(const boost::json::object& ci) {
        boost::json::object result = ci;
        result["status"] = "success";
        result["id"] = "unknown";

        std::string message;
        std::string id;

        if (!addCiToCMDB(ci, message, id)) {
            result["status"] = "failure";
        }

        result["message"] = message;
        result["id"] = id;

        return result;
    }

    boost::json::object DataStore::addCis(const boost::json::array& cis) {
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

            if (addCiToCMDB(ci, message, ciId)) {
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

    bool DataStore::addRelationshipToCMDB(const json::object& ci, std::string& message) {
        if (!ci.contains("source") || !ci.contains("destination") || !ci.contains("type")) {
            message = "Не запонены обязательные поля.";
            return false;
        }

        std::string source = boost::json::value_to<std::string>(ci.at("source"));
        std::string destination = boost::json::value_to<std::string>(ci.at("destination"));
        std::string type = boost::json::value_to<std::string>(ci.at("type"));
        
        if (!cmdb_.addRelationship(source, destination, type)) {
            message = "НЕ добавлено";    
            return false;
        }

        message = "добавлено";
        return true;
    }

    boost::json::object DataStore::addRelationship(const boost::json::object& relationship) {
        boost::json::object result;

        result["relationship"] = relationship;
        result["status"] = "success";

        std::string message;
        std::string from;
        std::string to;

        if (!addRelationshipToCMDB(relationship, message)) {
            result["status"] = "failure";
        }

        result["message"] = message;

        return result;
    }

    boost::json::object DataStore::addRelationships(const json::array& relationships) {
        boost::json::object result;
        boost::json::array rel_add;
        int addedCount = 0;

        for (const auto& relationshipValue : relationships) {
            boost::json::object entry;

            if (!relationshipValue.is_object()) {
                entry["relationship"] = relationshipValue;
                entry["message"] = "Элемент не является объектом JSON.";

                rel_add.push_back(entry);
                continue;
            }

            const boost::json::object& rel = relationshipValue.as_object();
            std::string message;

            if (addRelationshipToCMDB(rel, message)) {
                ++addedCount;
            }

            entry["relationship"] = relationshipValue;
            entry["message"] = message;

            rel_add.push_back(entry);
        }

        result["rels_add"] = rel_add;

        boost::json::object info;
        info["total"] = static_cast<int>(rel_add.size());
        info["added"] = addedCount;
        result["info"] = info;

        result["status"] = addedCount > 0 ? "success" : "failure";

        return result;
    }

    json::object DataStore::deleteLevel(int id) {
        json::object result;

        result["status"] = "success";
        result["message"] = "Удален";

        if (!cmdb_.removeLevel(id)) {
            result["status"] = "failure";
            result["error"] = "Ошибка удаления уровня: " + id;
        }

        return result;
    }

    json::object DataStore::deleteCi(const std::string& id) {
        json::object result;

        result["status"] = "success";
        result["message"] = "Удален";

        if (!cmdb_.removeCI(id)) {
            result["status"] = "failure";
            result["error"] = "Ошибка удаления CI: " + id;
        }

        return result;
    }

    boost::json::object DataStore::deleteRelationships(const std::map<std::string, std::string>& filters) {
        json::object result;

        result["status"] = "success";
        result["message"] = "удалено";

        if (filters.count("source") == 0 || filters.count("destination") == 0) {
            result["status"] = "failure";
            result["message"] = "Не запонены обязательные поля.";

            return result;
        }

        bool deleted = false;

        result["source"] = filters.at("source");
        result["destination"] = filters.at("destination");

        if (!filters.count("type") == 0) {
            deleted = cmdb_.removeRelationship(filters.at("source"), filters.at("destination"), filters.at("type"));
            result["type"] = filters.at("type");
        }

        deleted = cmdb_.removeRelationship(filters.at("source"), filters.at("destination"));

        if (!deleted) {
            result["status"] = "failure";
            result["message"] = "НЕ удалено";
        }

        return result;
    }

    bool DataStore::updateCiInCMDB(const json::object& ci, std::string& message, std::string& ciId) {
        if (!ci.contains("id")) {
            message = "Не запонен ID.";
            return false;
        }

        ciId = boost::json::value_to<std::string>(ci.at("id"));

        auto current_ci = cmdb_.getCI(ciId);

        if (current_ci == nullptr) {
            message = "Не найден ID.";
            return false;
        }

        return cmdb_.updateCI(current_ci, ci, message);
    }

    json::object DataStore::updateCi(const json::object& ci) {
        boost::json::object result;
        result["ci"] = ci;
        result["status"] = "success";
        result["id"] = "unknown";

        std::string message;
        std::string id;

        if (!updateCiInCMDB(ci, message, id)) {
            result["status"] = "failure";
        }

        result["message"] = message;
        result["id"] = id;

        return result;
    }

    json::object DataStore::updateCis(const json::array& cis) {
        boost::json::object result;
        boost::json::array cis_add;
        int updatedCount = 0;

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

            if (updateCiInCMDB(ci, message, ciId)) {
                ++updatedCount;
            }

            entry["id"] = ciId;
            entry["message"] = message;

            cis_add.push_back(entry);
        }

        result["cis_add"] = cis_add;

        boost::json::object info;
        info["total"] = static_cast<int>(cis.size());
        info["updated"] = updatedCount;
        result["info"] = info;

        result["status"] = updatedCount > 0 ? "success" : "failure";

        return result;
    }

    json::object DataStore::updateLevel(const json::object& level) {
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

        return result;
    }

    json::object DataStore::getPropsList() {
        json::object result;
        
        result["properties"] = cmdb_.getProps();

        return result;
    }

    bool DataStore::isCIexists(std::string id) {
        return cmdb_.getCI(id) != nullptr ? true : false;
    }
