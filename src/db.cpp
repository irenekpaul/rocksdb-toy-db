#include "db.h"
#include "sst.h"
#include <iostream>
#include <fstream>
#include <filesystem>

DB::DB(const std::string& wal_path)
    : wal(wal_path), manifest("data/manifest.json"), seq_num(0) {}

void DB::put(const std::string& key, const std::string& value) {
    seq_num++;
    wal.append_put(seq_num, key, value);
    memtable.put(key, value, seq_num);
}

std::optional<std::string> DB::get(const std::string& key) {
    auto mem_val = memtable.get(key);
    if (mem_val.has_value()) return mem_val;

    const auto& ssts = manifest.get_sst_files();
    for (auto it = ssts.rbegin(); it != ssts.rend(); ++it) {
        auto sst_val = SSTReader::get_from_sst(*it, key);
        if (sst_val.has_value()) return sst_val;
    }

    return std::nullopt;
}


void DB::dump_memtable() const {
    memtable.dump();
}

void DB::flush() {
    if (memtable.data().empty()) {
        std::cout << "[Nothing to flush]" << std::endl;
        return;
    }

    std::map<std::string, uint64_t> seq_map;
    for (const auto& [key, val_pair] : memtable.data()) {
        seq_map[key] = val_pair.second;
    }

    std::string filename = "data/sst_" + std::to_string(seq_num) + ".sst";
    SSTWriter::write_to_sst(memtable.data(), filename);
    manifest.add_sst(filename, seq_num);
    memtable.clear();

    if (manifest.get_last_flushed_seq() >= seq_num) {
        std::remove("data/wal.log");
        std::cout << "[WAL deleted after flush]" << std::endl;
    }
}

void DB::recover() {
    manifest.load();

    for (const auto& sst : manifest.get_sst_files()) {
        std::ifstream ifs(sst);
        std::string key, value;
        uint64_t seq;
    
        while (ifs >> key >> value >> seq) {
            memtable.put(key, value, seq);
        }
    }

    auto entries = wal.read_all();
    uint64_t flushed_seq = manifest.get_last_flushed_seq();

    for (const auto& [seq, key, value] : entries) {
        if (seq > flushed_seq) {
            if (value == "__DELETE__") {
                memtable.del(key, seq);
            } else {
                memtable.put(key, value, seq);
            }
        }
        seq_num = std::max(seq_num, seq);
    }

        std::cout << "[Recovery complete]" << std::endl;
    }

void DB::del(const std::string& key) {
    seq_num++;
    wal.append_delete(seq_num, key);
    memtable.del(key, seq_num);
}

void DB::reset_db() {
    std::cout << "[Resetting database files...]" << std::endl;

    if (!std::filesystem::exists("data")) {
        std::cout << "[Nothing to reset]" << std::endl;
        return;
    }

    std::filesystem::remove("data/wal.log");
    std::filesystem::remove("data/manifest.json");

    for (const auto& entry : std::filesystem::directory_iterator("data")) {
        if (entry.path().extension() == ".sst") {
            std::filesystem::remove(entry.path());
        }
    }

    std::cout << "[All data wiped]" << std::endl;
}

size_t DB::wal_size() const {
    std::ifstream f(wal_path, std::ios::binary | std::ios::ate);
    return f.tellg();  // Returns file size in bytes
}
