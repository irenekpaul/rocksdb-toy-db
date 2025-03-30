#include "manifest.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

Manifest::Manifest(const std::string& path) : filepath(path), last_flushed_seq(0) {}

void Manifest::load() {
    std::ifstream ifs(filepath);
    if (!ifs) return;
    json j; ifs >> j;
    last_flushed_seq = j["last_flushed_seq"];
    sst_files = j["sst_files"].get<std::vector<std::string>>();
}

void Manifest::save() {
    json j;
    j["last_flushed_seq"] = last_flushed_seq;
    j["sst_files"] = sst_files;
    std::ofstream ofs(filepath);
    ofs << j.dump(4);
}

void Manifest::add_sst(const std::string& sst_file, uint64_t max_seq) {
    sst_files.push_back(sst_file);
    last_flushed_seq = max_seq;
    save();
}

uint64_t Manifest::get_last_flushed_seq() const {
    return last_flushed_seq;
}

const std::vector<std::string>& Manifest::get_sst_files() const {
    return sst_files;
}
