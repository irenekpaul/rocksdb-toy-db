#pragma once

#include <string>
#include <vector>

struct SSTMeta {
    std::string filename;
    uint64_t seq_num;
    int level;
    std::string min_key;
    std::string max_key;
};

class Manifest {
public:
    explicit Manifest(const std::string& path);
    void load();
    void save();

    void add_sst(const std::string& filename, uint64_t seq_num, int level,
        const std::string& min_key, const std::string& max_key);
    uint64_t get_last_flushed_seq() const;
    const std::vector<SSTMeta>& get_sst_files() const;
    void remove_ssts(const std::vector<std::string>& filenames);

private:
    std::string filepath;
    uint64_t last_flushed_seq;
    std::vector<SSTMeta> sst_files;
};
