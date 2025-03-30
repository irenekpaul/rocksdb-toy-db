#pragma once

#include <string>
#include "memtable.h"
#include "wal.h"
#include "sst.h"
#include "manifest.h"


class DB {
public:
    explicit DB(const std::string& wal_path);
    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    void recover();
    void dump_memtable() const;
    void flush();
    void del(const std::string& key);
    static void reset_db();

private:
    Memtable memtable;
    WAL wal;
    uint64_t seq_num;
    Manifest manifest;
};
