#pragma once

#include <string>
#include <vector>
#include <tuple>

class WAL {
public:
    explicit WAL(const std::string& path);
    void append_put(uint64_t seq_num, const std::string& key, const std::string& value);
    std::vector<std::tuple<uint64_t, std::string, std::string>> read_all();
    void append_delete(uint64_t seq_num, const std::string& key);


private:
    std::string filepath;
};
