#include "rangeiterator.h"

RangeIterator::RangeIterator(const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& mem,
                             const std::vector<SSTMeta>& ssts,
                             const std::string& start, const std::string& end)
    : range_end(end) {

    mem_it = mem.lower_bound(start);
    mem_end = mem.upper_bound(end);

    for (const auto& sst : ssts) {
        if (sst.max_key < start || sst.min_key > end) continue;
        std::ifstream* ifs = new std::ifstream(sst.filename);
        if (!ifs->is_open()) {
            delete ifs;
            continue;
        }
        std::string key, value;
        uint64_t seq;
        if (*ifs >> key >> value >> seq) {
            if (key >= start && key <= end) {
                pq.push({key, value, seq, ifs});
                open_files.push_back(ifs);
            } else {
                delete ifs;
            }
        }
    }
}

RangeIterator::~RangeIterator() {
    for (auto f : open_files) {
        if (f) {
            f->close();
            delete f;
        }
    }
}

bool RangeIterator::has_next() const {
    return mem_it != mem_end || !pq.empty();
}

std::pair<std::string, std::string> RangeIterator::next() {
    std::string next_key;
    std::string next_val;
    uint64_t next_seq = 0;

    if (mem_it != mem_end) {
        if (mem_it->second.first.has_value()) {
            next_key = mem_it->first;
            next_val = mem_it->second.first.value();
            next_seq = mem_it->second.second;
        }
        ++mem_it;
    }

    if (!pq.empty()) {
        auto top = pq.top(); pq.pop();
        if (returned_keys.count(top.key) == 0 && top.key >= next_key) {
            next_key = top.key;
            next_val = top.value;
            next_seq = top.seq_num;
        }
        std::string k, v;
        uint64_t s;
        if (*(top.file) >> k >> v >> s) {
            if (k <= range_end) {
                pq.push({k, v, s, top.file});
            }
        }
    }

    returned_keys.insert(next_key);
    return {next_key, next_val};
}
