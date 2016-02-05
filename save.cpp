#include <iostream>
#include <fstream>
#include <stdexcept>
#include "markov.hpp"
#include <cstring>

unsigned char newkey = '\x11';
unsigned char newval = '\x12';
unsigned char sent_starter = '\x13';
void Markov::DB::doSavingThings(void) {
    std::fstream file;
    file.open("./example.mkdb", std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "could not open: " << std::strerror(errno) << "\n";
        return;
    }
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
        }
        std::cout << "SAVE: saved key " << pair.first << "\n";
        file << "\n";
    }
};
void Markov::DB::doReadingThings(void) {
    std::fstream file;
    file.open("./example.mkdb", std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: " << std::strerror(errno) << "\n";
        throw std::runtime_error("Unable to open file");
    }
    while (!file.eof()) {
        std::string str;
        getline(file, str);
        bool ss = false, val = false;
        if (str[0] != newkey) {
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
                    if (ss) insertSentenceStarter(kv.first);
                }
                else {
                    insertHunk(kv.first, kv.second);
                    kv.second = "";
                }
                continue;
            }
            if (val) kv.second = kv.second + c;
            else kv.first = kv.first + c;
        }
        insertHunk(kv.first, kv.second);
    }
}
