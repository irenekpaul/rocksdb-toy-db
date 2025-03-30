#pragma once

#include <string>
#include <vector>

class Manifest {
public:
    explicit Manifest(const std::string& path);
    void load();
    void save();

    void add_sst(const std::string& sst_file, uint64_t max_seq);
    uint64_t get_last_flushed_seq() const;
    const std::vector<std::string>& get_sst_files() const;

private:
    std::string filepath;
    uint64_t last_flushed_seq;
    std::vector<std::string> sst_files;
};
