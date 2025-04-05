#pragma once

#include <boost/json.hpp>
#include <string>
#include <map>
#include <unordered_map>
#include <thread>
#include "../../CMDB/CMDB.h"

namespace json = boost::json;

class DataStore {
public:
    DataStore(cmdb::CMDB& cmdb);

    // int AddRecord(const json::object& record);
    // bool UpdateRecord(int id, const json::object& record);
    // json::object GetAllRecords() const;

    json::object GetAllRecords();

    json::array GetAllLevels();

    json::object GetLevel(int id);

    json::array GetCi(const std::map<std::string, std::string>& filters);

    json::array GetRelationships(const std::map<std::string, std::string>& filters);

    json::object AddLevel(const json::object& level);

    boost::json::object AddCi(const json::object& ci);

    boost::json::object AddCis(const json::array& cis);

    bool AddRelationshipToCMDB(const json::object& ci, std::string& message);

    boost::json::object AddRelationship(const boost::json::object& relationships);
    
    boost::json::object AddRelationships(const json::array& relationships);

    json::object DeleteLevel(int id);

    json::object DeleteCi(const std::string& id);

    void DeleteRelationships(const std::map<std::string, std::string>& filters);

    json::object UpdateCi(const json::object& ci);

    json::object UpdateCis(const json::array& cis);

    json::object UpdateLevel(const json::object& level);

    json::object GetPropsList();

    int AddRecord(const json::object& record);

    bool UpdateRecord(int id, const json::object& record);

private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_;
    cmdb::CMDB& cmdb_;

    bool AddCiToCMDB(const json::object& ci, std::string& message, std::string& ciId);
    bool UpdateCiInCMDB(const json::object& ci, std::string& message, std::string& ciId);
    bool isCIexists(std::string id);
};
