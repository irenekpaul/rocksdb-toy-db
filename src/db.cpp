#include "db.h"
#include "sst.h"
#include "rangeiterator.h"
#include <iostream>
#include <fstream>
#include <filesystem>

DB::DB(const std::string& wal_path)
    : wal(wal_path), manifest("/Users/irenekomalp/Documents/spring2025/Adv DB Impl/project/data/manifest.json"), seq_num(0) {}

void DB::put(const std::string& key, const std::string& value) {
    seq_num++;
    wal.append_put(seq_num, key, value);
    memtable.put(key, value, seq_num);
    std::cout << "[DEBUG] DB::put -> wal.append_put(" << seq_num << ", " << key << ", " << value << ")" << std::endl;

}

std::optional<std::string> DB::get(const std::string& key) {
    auto mem_val = memtable.get(key);
    if (mem_val.has_value()) return mem_val;

    const auto& ssts = manifest.get_sst_files();
    for (auto it = ssts.rbegin(); it != ssts.rend(); ++it) {
        auto sst_val = SSTReader::get_from_sst(it->filename, key);
        if (sst_val.has_value() && sst_val.value() != "__DELETE__") {
            return sst_val;
        }
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

    std::map<std::string, std::pair<std::optional<std::string>, uint64_t>> live_data;
    for (const auto& [key, val_pair] : memtable.data()) {
        if (val_pair.first.has_value()) {
            live_data[key] = val_pair;
        }
    }

    if (live_data.empty()) {
        std::cout << "[Flush skipped: only tombstones]" << std::endl;
        return;
    }

    std::string filename = "data/L0_sst_" + std::to_string(seq_num) + ".sst";
    SSTWriter::write_to_sst(live_data, filename);

    std::string min_key = live_data.begin()->first;
    std::string max_key = live_data.rbegin()->first;

    manifest.add_sst(filename, seq_num, 0, min_key, max_key);
    manifest.save();

    memtable.clear();

    if (manifest.get_last_flushed_seq() >= seq_num) {
        std::remove("data/wal.log");
        std::cout << "[WAL deleted after flush]" << std::endl;
    }

    std::cout << "[Flushed to " << filename << "]" << std::endl;
}

void DB::recover() {
    manifest.load();

    for (const auto& sst : manifest.get_sst_files()) {
        std::cout << "[DEBUG] Replaying SST: " << sst.filename << std::endl;
        std::ifstream ifs(sst.filename);
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

<<<<<<< Updated upstream
    std::cout << "[All data wiped]" << std::endl;
}
=======
    std::cout << "all data wiped" << std::endl;
}

size_t DB::wal_size() {
    return wal.read_all().size();
}


void DB::compact_level(int from_level) {
    const auto& sst_files = manifest.get_sst_files();

    std::vector<std::string> level_files;
    std::vector<SSTMeta> matching;
    for (const auto& meta : sst_files) {
        if (meta.level == from_level) {
            level_files.push_back(meta.filename);
            matching.push_back(meta);
        }
    }

    if (level_files.size() < 2) {
        std::cout << "compaction skipped: less than 2 files in L" << from_level << "]" << std::endl;
        return;
    }

    std::map<std::string, std::pair<std::string, uint64_t>> merged;
    for (const auto& file : level_files) {
        auto contents = SSTReader::read_sst(file);
        for (const auto& [key, val_seq] : contents) {
            const auto& [val, seq] = val_seq;
            if (merged.find(key) == merged.end() || seq > merged[key].second) {
                merged[key] = {val, seq};
            }
        }
    }

    for (auto it = merged.begin(); it != merged.end();) {
        if (it->second.first == "__DELETE__") {
            it = merged.erase(it);
        } else {
            ++it;
        }
    }

    int to_level = from_level + 1;
    int num_partitions = 4;
    std::vector<std::vector<std::pair<std::string, std::pair<std::string, uint64_t>>>> buckets(num_partitions);

    size_t count = 0;
    for (const auto& [key, val_seq] : merged) {
        buckets[count % num_partitions].emplace_back(key, val_seq);
        count++;
    }

    for (int i = 0; i < num_partitions; ++i) {
        if (buckets[i].empty()) continue;

        std::string filename = "/Users/irenekomalp/Documents/spring2025/Adv DB Impl/project/data/L" + std::to_string(to_level) + "_sst_" + std::to_string(++seq_num) + ".sst";

        std::map<std::string, std::pair<std::optional<std::string>, uint64_t>> output;
        for (const auto& [key, val_seq] : buckets[i]) {
            output[key] = std::make_pair(val_seq.first, val_seq.second);
        }

        SSTWriter::write_to_sst(output, filename);

        auto min_key = buckets[i].front().first;
        auto max_key = buckets[i].back().first;

        manifest.add_sst(filename, seq_num, to_level, min_key, max_key);
    }

    manifest.remove_ssts(level_files);
    manifest.save();
    for (const auto& f : level_files) {
        std::remove(f.c_str());
    }

    std::cout << "compacted L" << from_level << " to L" << to_level << "]" << std::endl;
}


std::vector<std::pair<std::string, std::string>> DB::get_range(const std::string& start, const std::string& end) {
    std::map<std::string, std::pair<std::optional<std::string>, uint64_t>> merged;

    const auto& ssts = manifest.get_sst_files();
    for (auto it = ssts.rbegin(); it != ssts.rend(); ++it) {
        std::ifstream ifs(it->filename);
        std::string key, value;
        uint64_t seq;

        while (ifs >> key >> value >> seq) {
            if (key >= start && key <= end) {
                if (merged.count(key) == 0 || seq > merged[key].second) {
                    merged[key] = {value == "__DELETE__" ? std::nullopt : std::make_optional(value), seq};
                }
            }
        }
    }

    for (const auto& [key, val_pair] : memtable.data()) {
        const auto& [val, seq] = val_pair;
        if (key >= start && key <= end) {
            if (merged.count(key) == 0 || seq > merged[key].second) {
                merged[key] = {val, seq};
            }
        }
    }

    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& [key, val_pair] : merged) {
        const auto& [val, _] = val_pair;
        if (val.has_value()) {
            result.emplace_back(key, val.value());
        }
    }

    return result;
}
>>>>>>> Stashed changes
