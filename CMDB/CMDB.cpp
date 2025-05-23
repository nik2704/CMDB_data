#include "CMDB.h"

namespace cmdb {

const std::string LEVEL_0 = "Business Layer";
const std::string LEVEL_1 = "Application Layer";
const std::string LEVEL_2 = "Logical Layer";
const std::string LEVEL_3 = "Physical Layer";

std::unique_ptr<CMDB> CMDB::instance_;
std::once_flag CMDB::init_flag_;

CMDB& CMDB::getInstance(const std::string& filename) {
    std::call_once(init_flag_, [&]() {
        instance_.reset(new CMDB);
        instance_->filename_ = filename;

        instance_->startAutoSave();        

        if (std::filesystem::exists(filename)) {
            if (!instance_->loadFromFile()) {
                std::cerr << "Ошибка при загрузке данных из файла " << filename << std::endl;
            }
        } else {
            instance_->addLevel(LEVEL_0);
            instance_->addLevel(LEVEL_1);
            instance_->addLevel(LEVEL_2);
            instance_->addLevel(LEVEL_3);
        }
    });

    return *instance_;
}

CMDB::~CMDB() {
    {
        std::lock_guard<std::mutex> lock(stop_mutex_);
        stop_thread_ = true;
    }

    stop_condition_.notify_one();

    if (auto_save_thread_.joinable()) {
        auto_save_thread_.join();
    }

    if (modified_) {
        if (!saveToFile()) {
            std::cerr << "Ошибка: не удалось сохранить CMDB перед удалением!\n";
        }
    }
}

/*
*   BEGIN: Методы для работы с CI
*/

bool CMDB::addCI(
    const std::string& id,
    const std::string& name,
    const std::string& type,
    int level,
    const std::unordered_map<std::string, std::string>& properties
    )
{
    if (level >= static_cast<int>(levels_.size())) {
        return false;
    }
    std::lock_guard<std::mutex> lock(cis_mutex_);

    auto ci = std::make_shared<CI>(id, name, type, level, properties);
    all_cis_.push_back(ci);
    id_to_ci_[id] = ci;

    updatePropertiesMap(ci, ci->getProperties());

    modified_ = true;

    return true;
}

int CMDB::addLevel(const std::string& name) {
    auto it = std::find(levels_.begin(), levels_.end(), name);
    if (it != levels_.end()) {
        return std::distance(levels_.begin(), it);
    }

    levels_.push_back(name);
    modified_ = true;

    return levels_.size() - 1;
}

bool CMDB::renameLevel(size_t index, std::string new_name) {
    if (index >= levels_.size()) {
        return false;
    }

    levels_[index] = new_name; 

    modified_ = true;

    return true;
}

bool CMDB::removeLevel(size_t index) {
    if (index >= levels_.size()) {
        return false;
    }

    auto cis = getCIs(index);

    if (cis && !cis->empty()) {
        return false;
    }

    levels_.erase(levels_.begin() + index);
    modified_ = true;

    return true;
}

void CMDB::addLevels(const std::vector<std::string>* new_levels) {
    if (!new_levels) return;

    for (const auto& level : *new_levels) {
        addLevel(level);
    }

    modified_ = true;    
}

bool CMDB::setLevels(const std::vector<std::string>* new_levels) {
    if (!new_levels || !all_cis_.empty()) {
        return false;
    }

    levels_ = *new_levels;
    modified_ = true;

    return true;
}

bool CMDB::removeCI(const std::string& id) {
    auto it = id_to_ci_.find(id);
    if (it == id_to_ci_.end()) return false;

    std::lock_guard<std::mutex> lock(cis_mutex_);
    auto ciPtr = it->second;
    
    id_to_ci_.erase(it);

    all_cis_.erase(std::remove_if(all_cis_.begin(), all_cis_.end(), [&ciPtr](const CIPtr& ptr) { return ptr == ciPtr; }), all_cis_.end());

    removeRelationshipsForId(id);
    deletePropertiesMap(ciPtr, ciPtr->getProperties());

    modified_ = true;

    return true;
}

CMDB::CIPtr CMDB::getCI(const std::string& id) const {
    auto it = id_to_ci_.find(id);
    return (it != id_to_ci_.end()) ? it->second : nullptr;
}

template <typename Predicate>
std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIsImpl(Predicate pred) const {
    std::lock_guard<std::mutex> lock(cis_mutex_);

    auto result = std::make_shared<std::vector<CIPtr>>();

    for (const auto& ci_ptr : all_cis_) {
        if (pred(ci_ptr)) {
            result->push_back(ci_ptr);
        }
    }

    return result->empty() ? nullptr : result;
}


std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs() const {
    return getCIsImpl([](const CIPtr&) { return true; });
}

std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs(int level) const {
    return getCIsImpl([level](const CIPtr& ci) { return ci->getLevel() == level; });
}

std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::CMDB::getCIs(int level, const std::string& type) const {
    return getCIsImpl([level, type](const CIPtr& ci) { return ci->getLevel() == level && ci->getType() == type; });
}

std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs(const std::string& type) const {
    return getCIsImpl([type](const CIPtr& ci) { return ci->getType() == type; });
}

std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs(const std::map<std::string, std::string>& filters) const {
    auto result = std::make_shared<std::vector<CIPtr>>();
    std::string id;
    std::string name;
    std::string type;
    int level = -1;

    if (filters.empty()) {
        result = getCIs();
    } else {
        if (filters.count("id") > 0) {
            id = filters.at("id");
        }

        if (filters.count("name") > 0) {
            name = filters.at("name");
        }

        if (filters.count("type") > 0) {
            type = filters.at("type");
        }

        if (filters.count("level") > 0) {
            level = std::stoi(filters.at("level"));
        }

        std::shared_ptr<std::vector<cmdb::CMDB::CIPtr>> allCIs;
        
        if (filters.count("has_props") > 0) {
            auto has_props = splitAndDecode(filters.at("has_props"), ',');

            allCIs = getCIs(has_props);
        } else {
            allCIs = getCIs();
        }

        for (const auto& ci : *allCIs) {
            if ((id.empty() || ci->getId() == id) &&
                (name.empty() || ci->getName() == name) &&
                (type.empty() || ci->getType() == type) &&
                (level == -1 || ci->getLevel() == level)) {
                result->push_back(ci);
            }
        }        
    }

    return result;
}

std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs(const std::vector<std::string>& props) const {
    auto result = std::make_shared<std::vector<CIPtr>>();
    std::unordered_set<std::string> seen_ids;

    std::unordered_set<CIPtr> candidate_cis;

    for (const auto& prop : props) {
        auto it = property_to_cis_.find(prop);
        if (it == property_to_cis_.end()) {
            return std::make_shared<std::vector<CIPtr>>();
        }

        if (candidate_cis.empty()) {
            candidate_cis.insert(it->second.begin(), it->second.end());
        } else {
            std::unordered_set<CIPtr> intersection;
            for (const auto& ci_ptr : it->second) {
                if (candidate_cis.count(ci_ptr)) {
                    intersection.insert(ci_ptr);
                }
            }
            
            candidate_cis = std::move(intersection);
            if (candidate_cis.empty()) {
                return std::make_shared<std::vector<CIPtr>>();
            }
        }
    }

    for (const auto& ci_ptr : candidate_cis) {
        const auto& ci_props = ci_ptr->getProperties();
        bool has_all = std::all_of(props.begin(), props.end(), [&](const std::string& prop) {
            return ci_props.find(prop) != ci_props.end();
        });

        if (has_all) {
            result->push_back(ci_ptr);
        }
    }

    return result;
}


std::shared_ptr<std::vector<CMDB::CIPtr>> CMDB::getCIs(const std::string& id, size_t steps) const {
    std::lock_guard<std::mutex> lock(cis_mutex_);
    std::lock_guard<std::mutex> lock_dependencies(dependencies_mutex_);

    auto start_ci = getCI(id);
    if (!start_ci || steps == 0) {
        return std::make_shared<std::vector<CIPtr>>();
    }

    std::unordered_set<std::string> visited;
    std::queue<std::pair<std::string, size_t>> queue;

    visited.insert(id);
    queue.push({id, 0});

    auto result = std::make_shared<std::vector<CIPtr>>();

    while (!queue.empty()) {
        auto [current_id, depth] = queue.front();
        queue.pop();

        if (depth == steps) {
            auto ci = getCI(current_id);
            if (ci) {
                result->push_back(ci);
            }
            continue;
        }

        auto range = relationships_.equal_range(current_id);
        
        for (auto it = range.first; it != range.second; ++it) {
            const auto& next_id = it->second.getDestination();
            if (!visited.count(next_id)) {
                visited.insert(next_id);
                queue.push({next_id, depth + 1});
            }
        }
    }

    return result;
}

std::optional<std::string> CMDB::getLevelName(int index) {
    if (index < 0 || index >= static_cast<int>(levels_.size())) {
        return std::nullopt;
    }
    return levels_[index];
}

std::optional<int> CMDB::getLevelIndex(const std::string& name) {
    auto it = std::find(levels_.begin(), levels_.end(), name);
    if (it != levels_.end()) {
        return std::distance(levels_.begin(), it);
    }
    return std::nullopt;
}

std::shared_ptr<std::vector<std::string>> CMDB::getLevels() const {
    return std::make_shared<std::vector<std::string>>(levels_);
}


bool CMDB::updateCI(const std::string& id, const std::unordered_map<std::string, std::string>& properties) {
    auto ci = getCI(id);
    if (!ci) return false;
    
    ci->setProperties(properties);
    updatePropertiesMap(ci, ci->getProperties());

    modified_ = true;

    return true;
}

bool CMDB::updateCI(const std::string& id, const std::string& name, int level, const std::unordered_map<std::string, std::string>& properties) {
    auto ci = getCI(id);
    if (!ci) return false;
    
    ci->setName(name);
    ci->setLevel(level);
    ci->setProperties(properties);
    updatePropertiesMap(ci, ci->getProperties());

    modified_ = true;

    return true;
}

bool CMDB::updateCI (cmdb::CMDB::CIPtr current_ci, const boost::json::object &ci, std::string &message) {
    std::lock_guard<std::mutex> lock(cis_mutex_);

    auto current_props = current_ci->getProperties();

    if (current_ci->setProperties(ci, message)) {

        auto new_props = current_ci->getProperties();

        for (const auto& [key, value] : current_props) {
            if (new_props.find(key) == new_props.end()) {
                deletePropertyFromMap(current_ci, key);
            }
        }

        updatePropertiesMap(current_ci, new_props);

        message = "обновлен";
        modified_ = true;

        return true;
    } else {
        message = current_ci->getId() + " не обновлен";
        return false;
    }
}

bool CMDB::setProperty(const std::string& id, const std::string& property_name, const std::string& property_value) {
    auto ci = getCI(id);
    if (!ci) return false;

    ci->setProperty(property_name, property_value);
    addPropertyToMap(ci, property_name, property_value);

    modified_ = true;

    return true;
}


void CMDB::updatePropertiesMap(CIPtr ci, const std::unordered_map<std::string, std::string>& properties) {
    for (const auto& [property_name, property_value] : properties) {
        CIList& ci_list = property_to_cis_[property_name];

        if (std::find(ci_list.begin(), ci_list.end(), ci) == ci_list.end()) {
            ci_list.push_back(ci);
        }
    }
}

void CMDB::deletePropertyFromMap(CIPtr ci, const std::string& property_name) {
    auto it = property_to_cis_.find(property_name);
    if (it != property_to_cis_.end()) {
        CIList& ci_list = it->second;

        ci_list.erase(std::remove(ci_list.begin(), ci_list.end(), ci), ci_list.end());

        if (ci_list.empty()) {
            property_to_cis_.erase(it);
        }
    }
}

void CMDB::deletePropertiesMap(CIPtr ci, const std::unordered_map<std::string, std::string>& properties) {
    for (const auto& [property_name, property_value] : properties) {
        deletePropertyFromMap(ci, property_name);
    }
}
    
void CMDB::addPropertyToMap(CIPtr ci, const std::string& property_name, const std::string& /*property_value*/) {
    CIList& ci_list = property_to_cis_[property_name];

    if (std::find(ci_list.begin(), ci_list.end(), ci) == ci_list.end()) {
        ci_list.push_back(ci);
    }
}

void CMDB::restorePropertiesMap() {
    property_to_cis_.clear();

    for (const auto& ci : all_cis_) {
        if (ci) {
            updatePropertiesMap(ci, ci->getProperties());
        }
    }
}

boost::json::array CMDB::getProps() const {
    boost::json::array props_array;

    for (const auto& [property_name, ci_list] : property_to_cis_) {
        props_array.emplace_back(property_name);
    }

    return props_array;
}

// END: Методы для работы с CI

/*
*   BEGIN: Методы для работы со связями
*/

bool CMDB::addRelationship(const std::string& from_id, const std::string& to_id, const std::string& type) {
    auto from_ci = getCI(from_id);
    auto to_ci = getCI(to_id);

    if (!from_ci || !to_ci) {
        return false;
    }

    Relationship relationship(from_id, to_id, type);
    relationships_.emplace(from_ci->getId(), relationship);

    reverse_index_[to_id].insert(from_id);
    modified_ = true;

    return true;
}

bool CMDB::removeRelationship(const std::string& from_id, const std::string& to_id) {
    auto range = relationships_.equal_range(from_id);

    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.getDestination() == to_id) {
            relationships_.erase(it);
            reverse_index_[to_id].erase(from_id);

            if (reverse_index_[to_id].empty()) {
                reverse_index_.erase(to_id);
            }

            modified_ = true;
            return true;
        }
    }

    return false;
}

bool CMDB::removeRelationship(const std::string& from_id, const std::string& to_id, const std::string& type) {
    auto range = relationships_.equal_range(from_id);

    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.getDestination() == to_id && it->second.getType() == type) {
            relationships_.erase(it);
            reverse_index_[to_id].erase(from_id);

            if (reverse_index_[to_id].empty()) {
                reverse_index_.erase(to_id);
            }

            modified_ = true;
            return true;
        }
    }

    return false;
}

void CMDB::removeRelationshipsForId(const std::string& id) {
    for (auto it = relationships_.begin(); it != relationships_.end(); ) {
        if (it->first == id || it->second.getDestination() == id) {
            it = relationships_.erase(it);
            modified_ = true;
        } else {
            ++it;
        }
    }

    for (auto reverse_it = reverse_index_.begin(); reverse_it != reverse_index_.end(); ) {
        reverse_it->second.erase(id);
        if (reverse_it->second.empty()) {
            reverse_it = reverse_index_.erase(reverse_it);
            modified_ = true;
        } else {
            ++reverse_it;
        }
    }
}

template <typename Predicate>
std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationshipsImpl(Predicate pred) const {
    std::lock_guard<std::mutex> lock(dependencies_mutex_);

    auto result = std::make_shared<std::vector<RelationshipPtr>>();

    for (const auto& relationship_pair : relationships_) {
        auto relationship_ptr = std::make_shared<Relationship>(relationship_pair.second);
        if (pred(relationship_ptr)) {
            result->push_back(relationship_ptr);
        }
    }

    return result->empty() ? nullptr : result;
}

std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationships() const {
    return getRelationshipsImpl([](const RelationshipPtr& /*relationship*/) { return true; });
}

std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationships(const std::string& from_id) const {
    return getRelationshipsImpl([from_id](const RelationshipPtr& relationship) { return relationship->getSource() == from_id; });
}

std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationships(const std::string& from_id, const std::string& to_id) const {
    return getRelationshipsImpl([from_id, to_id](const RelationshipPtr& relationship) {
        bool from_match = (from_id == "*" || relationship->getSource() == from_id);
        bool to_match = (to_id == "*" || relationship->getDestination() == to_id);

        return from_match && to_match;
    });
}

std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationships(const std::map<std::string, std::string>& filters) const {
    auto result = std::make_shared<std::vector<RelationshipPtr>>();

    std::string source;
    std::string destination;
    std::string type;

    if (filters.empty()) {
        auto rels = getRelationships();
        if (!rels) return result;

        return rels;
    } else {
        if (filters.count("source") > 0) {
            source = filters.at("source");
        }

        if (filters.count("destination") > 0) {
            destination = filters.at("destination");
        }

        if (filters.count("type") > 0) {
            type = filters.at("type");
        }

        auto allrelationships = getRelationships();

        if (allrelationships) {
            for (const auto& rel : *allrelationships) {
                if ((source.empty() || rel->getSource() == source) &&
                    (destination.empty() || rel->getDestination() == destination) &&
                    (type.empty() || rel->getType() == type)) {
                    result->push_back(rel);
                }
            }
        }
    }

    return result;
}
std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getRelationships(const std::string& from_id,
    const std::string& to_id, const std::string& type) const {
    return getRelationshipsImpl([from_id, to_id, type](const RelationshipPtr& relationship) {
        bool from_match = (from_id == "*" || relationship->getSource() == from_id);
        bool to_match = (to_id == "*" || relationship->getDestination() == to_id);
        bool type_match = (type == "*" || relationship->getType() == type);

        return from_match && to_match && type_match;
    });
}

std::shared_ptr<std::vector<CMDB::RelationshipPtr>> CMDB::getDependentCIs(const std::string& id) const {
    auto dependent_cis = std::make_shared<std::vector<CMDB::RelationshipPtr>>();
    auto it = reverse_index_.find(id);

    if (it != reverse_index_.end()) {
        for (const auto& dep_id : it->second) {
            auto rel_it = relationships_.find(dep_id);

            while (rel_it != relationships_.end() && rel_it->first == dep_id) {
                dependent_cis->push_back(std::make_shared<Relationship>(rel_it->second));
                ++rel_it;
            }
        }
    }

    return dependent_cis;
}
// END: Методы для работы со связями 


/*
*   BEGIN: Методы для сохранения в файл
*/

void CMDB::markAsModified() {
    modified_ = true;
}

bool CMDB::saveCollection(std::ofstream& out, const CIList& collection) {
    size_t size = collection.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& item : collection) {
        if (!item->save(out)) return false; 
    }

    return out.good();
}

bool CMDB::saveCollection(std::ofstream& out, const std::vector<std::string>& collection) {
    size_t size = collection.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out) return false;

    for (const auto& item : collection) {
        size_t length = item.size();
        out.write(reinterpret_cast<const char*>(&length), sizeof(length));
        if (!out) return false;

        out.write(item.data(), length);
        if (!out) return false;
    }

    return out.good();
}

bool CMDB::saveCollection(std::ofstream& out, const RelationshipMap& collection) {
    size_t size = collection.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) return false;

    for (const auto& pair : collection) {
        size_t key_size = pair.first.size();
        out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));

        if (!out.good()) return false;

        out.write(pair.first.data(), key_size);
        if (!out.good()) return false;

        if (!pair.second.save(out)) {
            return false;
        }
    }

    return out.good();
}

bool CMDB::saveToFile() {
    return saveToFile(filename_);
}

bool CMDB::saveToFile(const std::string& filename) {
    std::string temp_filename = filename + ".tmp";

    std::lock_guard<std::mutex> lock(cis_mutex_);

    if (std::ifstream(filename)) {
        if (std::rename(filename.c_str(), temp_filename.c_str()) != 0) {
            std::cerr << "Ошибка: не удалось переименовать " << filename << " в " << temp_filename << "!\n";
            return false;
        }
    }

    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи!\n";
        std::rename(temp_filename.c_str(), filename.c_str());
        
        return false;
    }

    if (
        !saveCollection(out, levels_) ||
        !saveCollection(out, all_cis_) ||
        !saveCollection(out, relationships_)
        ) {
        std::cerr << "Ошибка: не удалось сохранить данные в " << filename << "!\n";
        out.close();

        std::remove(filename.c_str());
        std::rename(temp_filename.c_str(), filename.c_str());

        return false;
    }

    out.close();

    std::remove(temp_filename.c_str());

    std::cout << "CMDB успешно сохранена в " << filename << "\n";

    modified_ = false;

    return true;
}

bool CMDB::loadCollection(std::ifstream& in, CIList& collection) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    if (!in.good()) {
        std::cerr << "Error reading size from file.\n";
        return false;
    }

    std::cout << "Expected collection size: " << size << "\n";
    collection.resize(size);

    for (size_t i = 0; i < size; ++i) {
        collection[i] = std::make_shared<CI>();

        if (!collection[i]->load(in)) {
            std::cerr << "Error loading item " << i << " from file.\n";
            return false;
        }
    }

    std::cout << "Successfully loaded collection with " << size << " items.\n";
    return true;
}



bool CMDB::loadCollection(std::ifstream& in, std::vector<std::string>& collection) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!in) return false;

    collection.clear();
    collection.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        size_t length;
        in.read(reinterpret_cast<char*>(&length), sizeof(length));
        if (!in) return false;

        std::string item(length, '\0');
        in.read(&item[0], length);
        if (!in) return false;

        collection.push_back(std::move(item));
    }

    return in.good();
}

bool CMDB::loadCollection(std::ifstream& in, RelationshipMap& collection) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!in.good()) return false;

    collection.clear();

    for (size_t i = 0; i < size; ++i) {
        size_t key_size;
        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        if (!in.good()) return false;

        std::string key(key_size, '\0');
        in.read(&key[0], key_size);
        if (!in.good()) return false;

        Relationship relationship;
        if (!relationship.load(in)) return false;

        collection.emplace(std::move(key), std::move(relationship));
    }

    return in.good();
}

void CMDB::restoreIDtoCI() {
    id_to_ci_.clear();

    for (const auto& ci : all_cis_) {
        if (ci) {
            id_to_ci_[ci->getId()] = ci;
        }
    }
}

void CMDB::restoreReverseIndex() {
    reverse_index_.clear();

    for (const auto& [from_id, relationship] : relationships_) {
        const std::string& to_id = relationship.getDestination();
        reverse_index_[to_id].insert(from_id);
    }
}

bool CMDB::loadFromFile() {
    return loadFromFile(filename_);
}

bool CMDB::loadFromFile(const std::string& filename) {
    {
        std::lock_guard<std::mutex> lock(cis_mutex_);

        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения!\n";
            return false;
        }

        if (!loadCollection(in, levels_)) {
            std::cerr << "Ошибка: не удалось загрузить уровни из " << filename << "!\n";
            return false;
        }

        if (!loadCollection(in, all_cis_)) {
            std::cerr << "Ошибка: не удалось загрузить CIs из " << filename << "!\n";
            return false;
        }

        if (!loadCollection(in, relationships_)) {
            std::cerr << "Ошибка: не удалось загрузить связи из " << filename << "!\n";
            return false;
        }

        restoreIDtoCI();

        restorePropertiesMap();

        restoreReverseIndex();
    }

    std::cout << "CMDB загружена из " << filename << "\n";

    modified_ = false;

    return true;
}

void CMDB::startAutoSave() {
    auto_save_thread_ = std::thread(&CMDB::autoSaveLoop, this);
}

void CMDB::autoSaveLoop() {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    while (!stop_thread_) {
        if (!(stop_condition_.wait_for(lock, std::chrono::minutes(1)) == std::cv_status::timeout)) {
            if (stop_thread_) {
                break;
            }
        }

        if (modified_ && !saving_.exchange(true)) {
            saveToFile();
            modified_ = false;
            saving_ = false;
        }
    }
}

// END: Методы для сохранения в файл

std::string CMDB::urlDecode(const std::string& str) {
    std::ostringstream decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::istringstream hex_stream(str.substr(i + 1, 2));
            int hex = 0;
            if (hex_stream >> std::hex >> hex) {
                decoded << static_cast<char>(hex);
                i += 2;
            } else {
                decoded << '%';
            }
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    return decoded.str();
}   

std::vector<std::string> CMDB::splitAndDecode(const std::string& input, char a) {
    std::vector<std::string> result;
    std::istringstream ss(input);
    std::string token;

    while (std::getline(ss, token, a)) {
        result.push_back(urlDecode(token));
    }

    return result;
}

}
