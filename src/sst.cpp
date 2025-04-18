#include "sst.h"
#include <fstream>
#include <iostream>

void SSTWriter::write_to_sst(
    const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& data,
    const std::string& filepath
) {
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        std::cerr << "Error writing SST to " << filepath << std::endl;
        return;
    }

    for (const auto& [key, val_pair] : data) {
        const auto& [val, seq_num] = val_pair;
        if (val.has_value()) {
            ofs << key << " " << val.value() << " " << seq_num << "\n";
        }
    }

    std::cout << "[Flushed to " << filepath << "]" << std::endl;
}

std::optional<std::string> SSTReader::get_from_sst(const std::string& filepath, const std::string& key) {
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) return std::nullopt;

    std::string file_key, value;
    uint64_t seq;

    while (ifs >> file_key >> value >> seq) {
        if (file_key == key) {
            if (value == "__DELETE__") return std::nullopt;
            return value;
        }        
    }

    return std::nullopt;
}

std::map<std::string, std::pair<std::string, uint64_t>> SSTReader::read_sst(const std::string& filepath) {
    std::ifstream ifs(filepath);
    std::map<std::string, std::pair<std::string, uint64_t>> result;
    if (!ifs.is_open()) return result;

    std::string key, val;
    uint64_t seq;
    while (ifs >> key >> val >> seq) {
        result[key] = {val, seq};
    }

    return result;
}
