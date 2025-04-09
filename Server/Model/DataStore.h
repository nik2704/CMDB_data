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

    json::object getAllRecords();

    json::array getAllLevels();

    json::object getLevel(int id);

    json::array getCi(const std::map<std::string, std::string>& filters);

    json::array getRelationships(const std::map<std::string, std::string>& filters);

    json::object addLevel(const json::object& level);

    boost::json::object addCi(const json::object& ci);

    boost::json::object addCis(const json::array& cis);

    bool addRelationshipToCMDB(const json::object& ci, std::string& message);

    boost::json::object addRelationship(const boost::json::object& relationships);
    
    boost::json::object addRelationships(const json::array& relationships);

    json::object deleteLevel(int id);

    json::object deleteCi(const std::string& id);

    boost::json::object deleteRelationships(const std::map<std::string, std::string>& filters);

    json::object updateCi(const json::object& ci);

    json::object updateCis(const json::array& cis);

    json::object updateLevel(const json::object& level);

    json::object getPropsList();

private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_;
    cmdb::CMDB& cmdb_;

    bool addCiToCMDB(const json::object& ci, std::string& message, std::string& ciId);
    bool updateCiInCMDB(const json::object& ci, std::string& message, std::string& ciId);
    bool isCIexists(std::string id);
};
