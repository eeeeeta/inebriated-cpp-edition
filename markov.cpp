#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
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
    mdb.insertHunk("cats", "are");
    mdb.insertHunk("are", "great");
    mdb.insertHunk("are", "dogs");
    mdb.insertSentenceStarter("cats");
    mdb.build_sentence();
};
