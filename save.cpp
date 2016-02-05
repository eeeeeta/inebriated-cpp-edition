#include <iostream>
#include <fstream>
#include <stdexcept>
#include "markov.hpp"
#include <cstring>

unsigned char newkey = '\x11';
unsigned char newval = '\x12';
unsigned char sent_starter = '\x13';
bool Markov::DB::saveCorpus(std::string filename) {
    std::fstream file;
    file.open(filename, std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "SAVE: could not open " << filename << ": " << std::strerror(errno) << "\n";
        return false;
    }
    int c_sv = 0;
    for (auto pair : *kvdb) {
        bool is_ss = false;
        for (auto ss : *ssdb) {
            if (ss.first == pair.first)
                is_ss = true;
        }
        file << newkey;
        if (is_ss) file << sent_starter;
        file << pair.first;
        for (auto wit : *pair.second) {
            file << newval << wit->ptr->val;
            c_sv++;
        }
#ifdef DEBUG
        std::cout << "SAVE: saved key " << pair.first << "\n";
#endif
        file << "\n";
    }
    std::cout << "SAVE: saved database to " << filename << " (" << c_sv
              << " pairs saved)\n";
    return true;
};
bool Markov::DB::readCorpus(std::string filename) {
    std::fstream file;
    file.open(filename, std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "LOAD: could not open " << filename << ": " << std::strerror(errno) << "\n";
        return false;
    }
    int c_skip = 0, c_add = 0, c_ss = 0;
    while (!file.eof()) {
        std::string str;
        getline(file, str);
        bool ss = false, val = false;
        if (str[0] != newkey) {
            if (str.length() > 1) {
                c_skip++;
                std::cerr << "LOAD: warning: encountered malformed or unknown line:\n\t"
                          << str << "\n";
            }
            continue;
        }
        else str.erase(0, 1);
        std::pair<std::string, std::string> kv {std::string {""}, std::string {""}};
        for (auto c : str) {
            if (c == sent_starter) {
                ss = true;
                continue;
            }
            if (c == newval) {
                if (val == false) {
                    val = true;
                    if (ss) {
                        c_ss++;
                        insertSentenceStarter(kv.first);
                    }
                }
                else {
                    c_add++;
                    insertHunk(kv.first, kv.second);
                    kv.second = "";
                }
                continue;
            }
            if (val) kv.second = kv.second + c;
            else kv.first = kv.first + c;
        }
        c_add++;
        insertHunk(kv.first, kv.second);
    }
    std::cout << "LOAD: loaded " << filename << " (" << c_add << " hunks added, "
              << c_ss << " sentence starters, " << c_skip << " lines skipped)\n";
    return true;
}
