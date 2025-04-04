#include "db.h"
#include <iostream>
#include <sstream>
#include <chrono>

int main() {
    DB db("data/wal.log");
    std::string cmd;

    std::cout << "Toy RocksDB > ";
    while (std::getline(std::cin, cmd)) {
        if (cmd == "exit") break;
        else if (cmd == "recover") {
            auto start = std::chrono::high_resolution_clock::now();
            db.recover();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "[LOG] Recovery took " << duration << " microseconds.\n";

            // Optional: Write to CSV
            std::ofstream logf("recovery_log.csv", std::ios::app);
            logf << db.wal_size() << "," << duration << "\n";
            logf.close();
        }
        else if (cmd == "dump") db.dump_memtable();
        else if (cmd.rfind("put", 0) == 0) {
            std::istringstream iss(cmd);
            std::string tmp, key, value;
            iss >> tmp >> key >> value;
            if (!key.empty() && !value.empty()) db.put(key, value);
            else std::cout << "Usage: put <key> <value>\n";
        }
        else if (cmd == "flush") {
            db.flush();
        }     
        else if (cmd.rfind("del", 0) == 0) {
            std::istringstream iss(cmd);
            std::string tmp, key;
            iss >> tmp >> key;
            if (!key.empty()) db.del(key);
            else std::cout << "Usage: del <key>\n";
        }
        else if (cmd == "reset") {
            DB::reset_db();
        }
        else if (cmd.rfind("get", 0) == 0) {
            std::istringstream iss(cmd);
            std::string _, key;
            iss >> _ >> key;
            auto val = db.get(key);
            if (!val.has_value()) std::cout << "<not found>\n";
            else std::cout << val.value() << "\n";
        }                        
        else {
            std::cout << "Commands: put <k> <v>, get <k>, del, dump, recover, exit, reset\n";
        }
        std::cout << "Toy RocksDB > ";
    }
    return 0;
}
