#include "memtable.h"
#include <iostream>


void Memtable::clear() {
    table.clear();
}

void Memtable::dump() const {
    if (table.empty()) {
        std::cout << "[memtable empty]" << std::endl;
        return;
    }

    std::cout << "[memtable contents]" << std::endl;

    for (const auto& [key, val_pair] : table) {
        const auto& [val, seq] = val_pair;
        std::cout << key << " = " << (val ? val.value() : "<DELETED>") << " [" << seq << "]" << std::endl;
    }
    
}

const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& Memtable::data() const {
    return table;
}

void Memtable::put(const std::string& key, const std::string& value, uint64_t seq_num) {
    table[key] = {value, seq_num};
}

void Memtable::del(const std::string& key, uint64_t seq_num) {
    table[key] = {std::nullopt, seq_num};
}

bool Memtable::is_deleted(const std::string& key) const {
    auto it = table.find(key);
    return it != table.end() && !it->second.first.has_value();
}
std::optional<std::string> Memtable::get(const std::string& key) const {
    auto it = table.find(key);
    if (it == table.end() || !it->second.first.has_value()) {
        return std::nullopt;
    }
    return it->second.first.value();
}

