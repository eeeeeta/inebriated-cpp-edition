#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "markov.hpp"
#include "rand.hpp"
using namespace Markov;
using string = std::string;

using KeyI = KeyIter<Key>;

Key::Key(string istr) {
    val = istr;
    weight = cweight = 1;
    next = nullptr;
}
Key::~Key() {
    if (next != nullptr) delete next;
}
Key *Key::grow(string istr) {
    Key *kp = this;
    if (kp->val == istr) {
        kp->cweight++;
        std::cout << "MDB: was growing " << istr << ", but found existing\n";
        return kp;
    }
    while (kp->next != nullptr) {
        if (kp->val == istr) {
            kp->cweight++;
            std::cout << "MDB: was growing " << istr << ", but found existing\n";
            return kp;
        }
        kp = kp->next;
    }
    kp->next = new Key {istr};
    return kp->next;
}
int Key::getWeight(void) {
    return (cweight * 2) + weight;
}
int Key::length(void) {
    return std::distance(begin(), end());
}
KeyI Key::advance(int n) {
    auto iter = KeyI {this};
    while (n-- != 0) ++iter;
    return iter;
}
KeyI Key::begin(void) {
    return KeyI {this};
}
KeyI Key::end(void) {
    return KeyI {nullptr};
}


DB::DB() {
    kvdb = new std::unordered_map<string,Key *> {};
    ssdb = new std::vector<std::pair<string, int>> {};
    std::srand(std::time(0));
}
DB::~DB() {
    for (auto iter : *kvdb) {
        delete iter.second;
    }
    delete kvdb;
    delete ssdb;
}
void DB::recalcWeights(void) {
    for (auto iter : *kvdb) {
#ifdef DEBUG
        std::cout << "MDB: recalc'ing strand for key: " << iter.first << "\n";
#endif
        iter.second->weight = Rand::weightForKey(iter.second, kvdb);
    }
    for (auto& pair : *ssdb) {
#ifdef DEBUG
        std::cout << "MDB: recalc'ing weight for sentence starter: " << pair.first << "\n";
#endif
        if (kvdb->find(pair.first) == kvdb->end()) {
            std::cerr << "MDB: FIXME sentence starter pointed to nothing\n";
            /* FIXME: remove from database */
        }
        else pair.second = Rand::weightForKey(kvdb->find(pair.first)->second, kvdb);
    }
}
void DB::insertHunk(string k, string v) {
    auto iter = kvdb->find(k);
    if (iter != kvdb->end()) {
        (iter->second)->grow(v);
    }
    else {
        kvdb->insert({k, new Key {v}});
    }
#ifdef DEBUG
    std::cout << "MDB: inserted hunk " << k << " = " << v << "\n";
#endif
}
void DB::insertSentenceStarter(string ss) {
    ssdb->insert(ssdb->end(), {ss, 0});
#ifdef DEBUG
    std::cout << "MDB: inserted sentence starter " << ss << "\n";
#endif
}
void DB::_debugDB() {
    for (auto kv : *kvdb) {
        std::cout << "DBG: key " << kv.first << "\n";
        for (auto k : *(kv.second)) {
            std::cout << "     val " << k->ptr->val << " w" << k->ptr->weight << " cw"
                      << k->ptr->cweight << "\n";
        }
    }
}
string DB::build_sentence(void) {
    if (ssdb->size() == 0)
        throw std::invalid_argument("No sentence starters in database.");
    string startk = Rand::randomSsdbObj(ssdb).first;
    string ret = startk;
#ifdef DEBUG
    std::cout << "MDB: got sentence starter: " << startk << "\n";
#endif
    auto iter = kvdb->find(startk);
    while (iter != kvdb->end()) {
        string str = Rand::randomKey(iter->second)->val;
#ifdef DEBUG
        std::cout << "MDB: str " << str << "\n";
#endif
        ret = ret + " " + str;
        iter = kvdb->find(str);
    }
#ifdef DEBUG
    std::cout << "MDB: built sentence " << ret << "\n";
#endif
    return ret;
}
