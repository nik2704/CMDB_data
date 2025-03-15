#pragma once

#include <boost/json.hpp>
#include <unordered_map>
#include <thread>

namespace json = boost::json;

class DataStore {
public:
    int AddRecord(const json::object& record);
    bool UpdateRecord(int id, const json::object& record);
    json::object GetAllRecords() const;
private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_;
};
