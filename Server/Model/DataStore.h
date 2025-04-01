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

    void AddLevel(const json::object& level);

    boost::json::object AddCi(const json::object& ci);

     boost::json::object AddCis(const json::array& cis);

    void AddRelationships(const json::array& relationships);

    void DeleteLevel(int id);

    void DeleteCi(const std::map<std::string, std::string>& filters);

    void DeleteRelationships(const std::map<std::string, std::string>& filters);

    void UpdateCi(const json::object& ci);

    void UpdateCis(const json::array& cis);

    void UpdateLevel(const json::object& level);

    int AddRecord(const json::object& record);

    bool UpdateRecord(int id, const json::object& record);

private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_;
    cmdb::CMDB& cmdb_;

    bool isCIexists(std::string id);
};
