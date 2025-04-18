#include "db.h"
#include <iostream>
#include <sstream>
<<<<<<< Updated upstream
=======
#include <chrono>
#include <fstream>
>>>>>>> Stashed changes

int main() {
    DB db("/Users/irenekomalp/Documents/spring2025/Adv DB Impl/project/data/wal.log");

    std::string cmd;
    while (std::getline(std::cin, cmd)) {
<<<<<<< Updated upstream
        if (cmd == "exit") break;
        else if (cmd == "recover") db.recover();
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
=======
        if (cmd.empty()) continue;

        std::istringstream iss(cmd);
        std::string op;
        iss >> op;

        if (op == "put") {
            std::string key;
            iss >> key;
            std::string val;
            std::getline(iss, val);

            if (!val.empty() && val[0] == ' ') val = val.substr(1);
            if (!val.empty() && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.size() - 2);
            }

            std::cout << "[DEBUG] Received PUT for key=" << key << ", value=" << val << std::endl;
            db.put(key, val);
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "get") {
            std::string key;
            iss >> key;
            std::cout << "[DEBUG] Received GET for key=" << key << std::endl;
>>>>>>> Stashed changes
            auto val = db.get(key);
            if (val.has_value()) {
                std::cout << val.value() << std::endl;
            } else {
                std::cout << "NULL" << std::endl;
            }
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "get_range") {
            std::string start, end;
            iss >> start >> end;
            std::cout << "[DEBUG] Received GET_RANGE from " << start << " to " << end << std::endl;
            auto results = db.get_range(start, end);
            for (const auto& [k, v] : results) {
                std::cout << k << "=" << v << " ";
            }
            std::cout << std::endl << "[OK]" << std::endl;
        }

        else if (op == "del") {
            std::string key;
            iss >> key;
            std::cout << "[DEBUG] Received DEL for key=" << key << std::endl;
            db.del(key);
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "flush") {
            std::cout << "[DEBUG] FLUSH command received" << std::endl;
            db.flush();
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "reset") {
            std::cout << "[DEBUG] RESET command received" << std::endl;
            db.reset_db();
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "dump") {
            std::cout << "[DEBUG] DUMP command received" << std::endl;
            db.dump_memtable();
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "recover") {
            std::cout << "[DEBUG] RECOVER command received" << std::endl;
            db.recover();
            std::cout << "[OK]" << std::endl;
        }

        else if (op == "exit") {
            std::cout << "[DEBUG] EXIT command received" << std::endl;
            break;
        }
        else if (op == "compact_level") {
            std::string level_str;
            iss >> level_str;
            if (level_str.empty()) {
                std::cerr << "[ERROR] compact_level requires a level\n";
            } else {
                int level = std::stoi(level_str);
                std::cout << "[DEBUG] COMPACT_LEVEL " << level << std::endl;
                db.compact_level(level);
            }
            std::cout << "[OK]" << std::endl;
        }        
        else {
            std::cerr << "Unrecognized op: " << op << std::endl;
            return 1;
        }
    }

    return 0;
}
