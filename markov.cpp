#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <ctime>
using string = std::string;
class MarkovDB {
    std::unordered_multimap<string,string>* kvdb;
    std::vector<string> *ssdb;
public:
    MarkovDB() {
        kvdb = new std::unordered_multimap<string,string> {};
        ssdb = new std::vector<string> {};
        std::srand(std::time(0));
    }
    ~MarkovDB() {
        delete kvdb;
        delete ssdb;
    }
    void insertHunk(string k, string v) {
        kvdb->insert({k, v});
        std::cout << "MDB: inserted hunk " << k << " = " << v << "\n";
    }
    void insertSentenceStarter(string ss) {
        ssdb->insert(ssdb->end(), ss);
        std::cout << "MDB: inserted sentence starter " << ss << "\n";
    }
    string build_sentence(void) {
        if (ssdb->size() == 0) throw std::invalid_argument("no sentence starters yet");
        int ridx = std::rand() % ssdb->size();
        string ss = ssdb->at(ridx);
        string ret = ss;
        std::cout << "MDB: got sentence starter: " << ss << "\n";
        auto iters = kvdb->equal_range(ss);
        while (iters.first != kvdb->end()) {
            ridx = std::rand() % std::distance(iters.first, iters.second);
            int cidx = 0;
            for (auto iter = iters.first; iter != iters.second; ++iter) {
                if (cidx == ridx) {
                    std::cout << "MDB: add hunk " << iter->second << "\n";
                    ret = ret + " " + iter->second;
                    ss = iter->second;
                    break;
                }
                cidx++;
            };
            iters = kvdb->equal_range(ss);
        };
        std::cout << "MDB: sentence: " << ret << "\n";
        return ret;
    }
};

int main() {
    std::cout << "inebriated, C++ edition v0.1\nan eta thing <http://theta.eu.org>\n\n";
    MarkovDB mdb {};
    std::cout << "input sentences, newline-separated:\n";
    std::string sentence;
    while (!std::cin.eof()){
        std::string sentence;
        std::getline(std::cin, sentence);
        std::istringstream ss;
        ss.str(sentence);
        std::pair<std::string,std::string> kv {"", ""};
        std::string tok;
        int spaces = 0;
        bool first = true;
        while (std::getline(ss, tok, ' ')) {
            if (spaces++ < 2) kv.first = kv.first + (kv.first == "" ? "" : " ") + tok;
            else kv.second = kv.second + (kv.second == "" ? "" : " ") + tok;
            if (spaces == 2) {
                tok = "";
                if (first) {
                    first = false;
                    mdb.insertSentenceStarter(kv.first);
                }
            }
            else if (spaces == 4) {
                tok = "";
                spaces = 0;
                mdb.insertHunk(kv.first, kv.second);
                kv = {"", ""};
            }
        }
    }
    std::cin.clear();
    std::cout << "\nSentence input complete.\nGenerating sentences (Enter for new, Control-D to stop)\n\n";
    while (!std::cin.eof()) {
        mdb.build_sentence();
        string str;
        std::getline(std::cin, str);
    };
};
