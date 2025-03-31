#include "Relationship.h"

namespace cmdb {

Relationship::Relationship(const std::string& source, const std::string& destination, const std::string& type, double weight)
        : source_(source), destination_(destination), type_(type), weight_(weight) {}

std::string Relationship::getType() const { return type_; }
std::string Relationship::getSource() const { return source_; }
std::string Relationship::getDestination() const { return destination_; }
double Relationship::getWeight() const { return weight_; }

std::string Relationship::getCIasJSONstring() const {
    return boost::json::serialize(asJSON());
}

boost::json::object Relationship::asJSON() const {
    boost::json::object json_obj;
    json_obj["type"] = type_;
    json_obj["source"] = source_;
    json_obj["destination"] = destination_;
    json_obj["weight"] = weight_;

    return json_obj;
}

bool Relationship::save(std::ofstream& out) const {
    if (!out) {
        return false;
    }

    size_t typeLen = type_.size(), sourceLen = source_.size(), destLen = destination_.size();
    out.write(reinterpret_cast<const char*>(&typeLen), sizeof(typeLen));
    out.write(type_.data(), typeLen);
    out.write(reinterpret_cast<const char*>(&sourceLen), sizeof(sourceLen));
    out.write(source_.data(), sourceLen);
    out.write(reinterpret_cast<const char*>(&destLen), sizeof(destLen));
    out.write(destination_.data(), destLen);
    out.write(reinterpret_cast<const char*>(&weight_), sizeof(weight_));

    return !out.fail();
}

bool Relationship::load(std::ifstream& in) {
    if (!in) {
        return false;
    }

    size_t typeLen, sourceLen, destLen;
    in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
    if (in.fail()) return false;

    type_.resize(typeLen);
    in.read(&type_[0], typeLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&sourceLen), sizeof(sourceLen));
    if (in.fail()) return false;

    source_.resize(sourceLen);
    in.read(&source_[0], sourceLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&destLen), sizeof(destLen));
    if (in.fail()) return false;

    destination_.resize(destLen);
    in.read(&destination_[0], destLen);
    if (in.fail()) return false;

    in.read(reinterpret_cast<char*>(&weight_), sizeof(weight_));
    if (in.fail()) return false;

    return true;
}

} // namespace cmdb