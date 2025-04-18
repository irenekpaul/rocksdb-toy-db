#pragma once

#include <map>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include <optional>
#include <utility>
#include <fstream>
#include "manifest.h"
#include "sst.h"

struct IteratorItem {
    std::string key;
    std::string value;
    uint64_t seq_num;
    std::ifstream* file;

    bool operator>(const IteratorItem& other) const {
        return key > other.key;
    }
};

class RangeIterator {
public:
    RangeIterator(
        const std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>& mem,
        const std::vector<SSTMeta>& ssts,
        const std::string& start, const std::string& end
    );

    ~RangeIterator();

    bool has_next() const;
    std::pair<std::string, std::string> next();

private:
    std::map<std::string, std::pair<std::optional<std::string>, uint64_t>>::const_iterator mem_it, mem_end;
    std::priority_queue<IteratorItem, std::vector<IteratorItem>, std::greater<>> pq;
    std::string range_end;
    std::vector<std::ifstream*> open_files;
    std::set<std::string> returned_keys;
};
