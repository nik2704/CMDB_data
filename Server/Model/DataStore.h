#pragma once

#include <boost/json.hpp>
#include <unordered_map>
#include <thread>

namespace json = boost::json;

class DataStore {
public:
    static DataStore& getInstance();

    int AddRecord(const json::object& record);
    bool UpdateRecord(int id, const json::object& record);
    json::object GetAllRecords() const;
private:
    DataStore() = default;
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;

    std::unordered_map<int, std::unordered_map<std::string, std::string>> data_;
};
