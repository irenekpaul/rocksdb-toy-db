#include "manifest.h"
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


namespace nlohmann {
    void to_json(json& j, const SSTMeta& meta) {
        j = {
            {"filename", meta.filename},
            {"seq_num", meta.seq_num},
            {"level", meta.level},
            {"min_key", meta.min_key},
            {"max_key", meta.max_key}
        };
    }

    void from_json(const json& j, SSTMeta& meta) {
        j.at("filename").get_to(meta.filename);
        j.at("seq_num").get_to(meta.seq_num);
        j.at("level").get_to(meta.level);
        j.at("min_key").get_to(meta.min_key);
        j.at("max_key").get_to(meta.max_key);
    }
}

Manifest::Manifest(const std::string& path) : filepath(path), last_flushed_seq(0) {}

void Manifest::load() {
    std::ifstream ifs(filepath);
    if (!ifs) return;
    json j;
    ifs >> j;
    last_flushed_seq = j["last_flushed_seq"];
    sst_files = j["sst_files"].get<std::vector<SSTMeta>>();
}

void Manifest::save() {
    json j;
    j["last_flushed_seq"] = last_flushed_seq;
    j["sst_files"] = sst_files;
    std::ofstream ofs(filepath);
    ofs << j.dump(4);
}

void Manifest::add_sst(const std::string& filename, uint64_t seq_num, int level,
    const std::string& min_key, const std::string& max_key) {
    sst_files.push_back({filename, seq_num, level, min_key, max_key});
    if (seq_num > last_flushed_seq) {
        last_flushed_seq = seq_num;
    }
}

uint64_t Manifest::get_last_flushed_seq() const {
    return last_flushed_seq;
}

const std::vector<SSTMeta>& Manifest::get_sst_files() const {
    return sst_files;
}

void Manifest::remove_ssts(const std::vector<std::string>& filenames) {
    std::set<std::string> to_remove(filenames.begin(), filenames.end());
    std::vector<SSTMeta> updated;
    for (const auto& meta : sst_files) {
        if (to_remove.count(meta.filename) == 0) {
            updated.push_back(meta);
        }
    }
    sst_files = std::move(updated);
}