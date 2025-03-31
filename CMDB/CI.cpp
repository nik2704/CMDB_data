#include "CI.h"

namespace cmdb {

CI::CI(const std::string& id, const std::string& name, const std::string& type)
    : id_(id), name_(name), type_(type), level_(0) {}

CI::CI(const std::string& id, const std::string& name, const std::string& type,
    int level, const std::unordered_map<std::string, std::string>& properties)
    : id_(id), name_(name), type_(type), level_(level), properties_(properties) {}

std::string CI::getId() const { return id_; }
std::string CI::getName() const { return name_; }
std::string CI::getType() const { return type_; }
int CI::getLevel() const { return level_; }
const std::unordered_map<std::string, std::string>& CI::getProperties() const { return properties_; }

std::string CI::getCIasJSONstring() const {
    return boost::json::serialize(asJSON());
}

boost::json::object CI::asJSON() const {
    boost::json::object json_obj;
    json_obj["id"] = id_;
    json_obj["name"] = name_;
    json_obj["type"] = type_;
    json_obj["level"] = level_;

    boost::json::object props;
    for (const auto& [key, value] : properties_) {
        props[key] = value;
    }

    json_obj["properties"] = std::move(props);

    return json_obj;
}


void CI::setName(const std::string& name) { name_ = name; }
void CI::setLevel(int level) { level_ = level; }
void CI::setProperty(const std::string& key, const std::string& value) {
    properties_[key] = value;
}
void CI::setProperties(const std::unordered_map<std::string, std::string>& properties) {
    properties_ = properties;
}

bool CI::hasProperty(const std::string& key) const {
    return properties_.find(key) != properties_.end();
}

std::optional<std::string> CI::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool CI::removeProperty(const std::string& key) {
    auto it = properties_.find(key);
    
    if (it != properties_.end()) {
        properties_.erase(it);
        return true;
    }

    return false;
}

bool CI::save(std::ofstream& out) const {
    if (!out) {
        return false;
    }

    size_t idLen = id_.size(), nameLen = name_.size(), typeLen = type_.size();
    out.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
    out.write(id_.data(), idLen);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(name_.data(), nameLen);
    out.write(reinterpret_cast<const char*>(&typeLen), sizeof(typeLen));
    out.write(type_.data(), typeLen);
    
    int level = level_;
    out.write(reinterpret_cast<const char*>(&level), sizeof(level));

    size_t propSize = properties_.size();
    out.write(reinterpret_cast<const char*>(&propSize), sizeof(propSize));
    for (const auto& [key, value] : properties_) {
        size_t keyLen = key.size(), valueLen = value.size();
        out.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        out.write(key.data(), keyLen);
        out.write(reinterpret_cast<const char*>(&valueLen), sizeof(valueLen));
        out.write(value.data(), valueLen);
    }

    return !out.fail();
}

bool CI::load(std::ifstream& in) {
    if (!in) {
        return false;
    }

    size_t idLen, nameLen, typeLen;

    in.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
    if (in.fail() || idLen == 0) return false;
    id_.resize(idLen);
    in.read(&id_[0], idLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (in.fail() || nameLen == 0) return false;
    name_.resize(nameLen);
    in.read(&name_[0], nameLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
    if (in.fail() || typeLen == 0) return false;
    type_.resize(typeLen);
    in.read(&type_[0], typeLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&level_), sizeof(level_));
    if (in.fail()) return false;

    size_t propSize;
    in.read(reinterpret_cast<char*>(&propSize), sizeof(propSize));
    if (in.fail()) return false;

    properties_.clear();
    for (size_t i = 0; i < propSize; ++i) {
        size_t keyLen, valueLen;
        std::string key, value;

        in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (in.fail()) return false;

        key.resize(keyLen);
        in.read(&key[0], keyLen);
        if (in.fail()) return false;

        in.read(reinterpret_cast<char*>(&valueLen), sizeof(valueLen));
        if (in.fail()) return false;
        
        value.resize(valueLen);
        in.read(&value[0], valueLen);
        if (in.fail()) return false;

        properties_[key] = value;
    }

    return !in.fail();  // Вернет false, если произошла ошибка при чтении
}


void CI::print() const {
    std::cout << "CI ID: " << id_ << "\n"
                << "Name: " << name_ << "\n"
                << "Type: " << type_ << "\n"
                << "Level: " << level_ << "\n"
                << "Properties:\n";
    for (const auto& property : properties_) {
        std::cout << "  " << property.first << ": " << property.second << "\n";
    }
}

}