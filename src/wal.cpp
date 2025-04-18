#include "wal.h"
#include <fstream>
#include <sstream>
#include <iostream>

WAL::WAL(const std::string& path) : filepath(path) {}

void WAL::append_put(uint64_t seq_num, const std::string& key, const std::string& value) {
    std::ofstream ofs(filepath, std::ios::app);
    ofs << "PUT " << seq_num << " " << key << " " << value << "\n";
    std::cout << "[DEBUG] WAL::append_put writing: PUT " << seq_num << " " << key << " " << value << std::endl;
}

std::vector<std::tuple<uint64_t, std::string, std::string>> WAL::read_all() {
    std::vector<std::tuple<uint64_t, std::string, std::string>> entries;
    std::ifstream ifs(filepath);
    std::string line;

    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string op;
        uint64_t seq;
        std::string key, value;
    
        iss >> op;
    
        if (op == "PUT" && iss >> seq >> key >> value) {
            std::cout << "[WAL] Parsed: " << op << " " << seq << " " << key << " " << value << std::endl;
            entries.emplace_back(seq, key, value);
        } else if (op == "DEL" && iss >> seq >> key) {
            std::cout << "[WAL] Parsed: " << op << " " << seq << " " << key << std::endl;
            entries.emplace_back(seq, key, "__DELETE__");
        } else {
            std::cerr << "[WAL] Could not parse line: " << line << std::endl;
        }
    }    
    return entries;
}


void WAL::append_delete(uint64_t seq_num, const std::string& key) {
    std::ofstream ofs(filepath, std::ios::app);
    ofs << "DEL " << seq_num << " " << key << "\n";
}
