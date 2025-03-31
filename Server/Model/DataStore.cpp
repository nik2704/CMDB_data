#include "DataStore.h"

DataStore::DataStore(cmdb::CMDB& cmdb) : cmdb_(cmdb) {}

int DataStore::AddRecord(const json::object& record) {
    int new_id = data_.size() + 1;
    for (auto& [key, value] : record) {
        data_[new_id][key] = value.as_string();
    }
    return new_id;
}

bool DataStore::UpdateRecord(int id, const json::object& record) {
    if (data_.find(id) == data_.end()) return false;
    for (auto& [key, value] : record) {
        data_[id][key] = value.as_string();
    }
    return true;
}

json::object DataStore::GetAllRecords() const {
    json::object result;
    for (const auto& [id, record] : data_) {
        json::object record_json;
        for (const auto& [key, value] : record) {
            record_json[key] = value;
        }
        result[std::to_string(id)] = record_json;
    }
    return result;
}
