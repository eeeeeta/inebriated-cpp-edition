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
template <typename T> class KeyIter : public std::iterator<std::forward_iterator_tag, T> {
public:
    T* ptr;
    using difference_type = int;
    KeyIter(T* iptr) {
        ptr = iptr;
    }
    ~KeyIter() {};
    KeyIter* operator++(void) {
        ptr = ptr->next;
        return this;
    }
    bool operator==(KeyIter a) {
        return a.ptr == ptr;
    }
    bool operator!=(KeyIter a) {
        return a.ptr != ptr;
    }
    KeyIter* operator*() {
        return this;
    }
};
class Key {
public:
    using iterator = KeyIter<Key>;

    Key *next;
    string val;
    int weight;

    Key(string istr) {
        val = istr;
        weight = 0;
        next = nullptr;
    }
    ~Key() {
        if (next != nullptr) delete next;
    }
    Key *grow(string istr) {
        Key *kp = this;
        while (kp->next != nullptr) kp = kp->next;
        kp->next = new Key {istr};
        return kp->next;
    }
    iterator advance(int n) {
        auto iter = iterator {this};
        while (n-- != 0) ++iter;
        return iter;
    }
    iterator begin(void) {
        return iterator {this};
    }
    iterator end(void) {
        return iterator {nullptr};
    }
};
class MarkovDB {
    std::unordered_map<string,Key *>* kvdb;
    std::vector<string> *ssdb;
public:
    MarkovDB() {
        kvdb = new std::unordered_map<string,Key *> {};
        ssdb = new std::vector<string> {};
        std::srand(std::time(0));
    }
    ~MarkovDB() {
        for (auto iter : *kvdb) {
            delete iter.second;
        }
        delete kvdb;
        delete ssdb;
    }
    void insertHunk(string k, string v) {
        auto iter = kvdb->find(k);
        if (iter != kvdb->end()) {
            (iter->second)->grow(v);
        }
        else {
            kvdb->insert({k, new Key {v}});
        }
        std::cout << "MDB: inserted hunk " << k << " = " << v << "\n";
    }
    void insertSentenceStarter(string ss) {
        ssdb->insert(ssdb->end(), ss);
        std::cout << "MDB: inserted sentence starter " << ss << "\n";
    }
    string build_sentence(void) {
        if (ssdb->size() == 0)
            throw std::invalid_argument("No sentence starters in database.");
        string startk = ssdb->at(std::rand() % ssdb->size());
        string ret = startk;
        std::cout << "MDB: got sentence starter: " << startk << "\n";
        auto iter = kvdb->find(startk);
        while (iter != kvdb->end()) {
            int dist = std::distance(iter->second->begin(), iter->second->end());
            int rn = std::rand() % dist;
            std::cout << "MDB: dist " << dist << " rn " << rn << "\n";
            string str = iter->second->advance(rn).ptr->val;
            std::cout << "MDB: str " << str << "\n";
            ret = ret + " " + str;
            iter = kvdb->find(str);
        }
        std::cout << "MDB: built sentence " << ret << "\n";
        return ret;
    }
};

int main() {
    std::cout << "inebriated, C++ edition v0.1.2\nan eta thing <http://theta.eu.org>\n\n";
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
