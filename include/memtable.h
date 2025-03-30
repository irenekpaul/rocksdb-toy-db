#pragma once

#include <string>
#include <map>

class Memtable {
public:
    void put(const std::string& key, const std::string& value, uint64_t seq_num);
    std::optional<std::string> get(const std::string& key) const;
    void del(const std::string& key, uint64_t seq_num);
    void dump() const;
    const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& data() const;
    void clear();
    bool is_deleted(const std::string& key) const;

private:
    std::map<std::string, std::pair<std::optional<std::string>, uint64_t>> table; 
};