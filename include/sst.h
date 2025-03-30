#pragma once

#include <string>
#include <map>
#include <optional>

class SSTWriter {
public:
    static void write_to_sst(
        const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& data,
        const std::string& filepath
    );
};

class SSTReader {
    public:
        static std::optional<std::string> get_from_sst(const std::string& filepath, const std::string& key);
    };
