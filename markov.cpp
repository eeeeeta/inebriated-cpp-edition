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

/**
 * Recalculate weightings for one 'strand' (linked list) of Key objects.
 */
void DB::_recalcOneStrand(Key *strand) {
    for (auto k : *(strand)) {
        std::cout << "MDB: value " << k->ptr->val << ":\n";
        auto iter = kvdb->find(k->ptr->val);
        if (iter == kvdb->end()) {
            k->ptr->weight = 0;
            std::cout << "- no results\n";
            return;
        }
        int dist = std::distance(iter->second->begin(), iter->second->end());
        std::cout << "- " << dist << " results\n";
        k->ptr->weight = dist;
    }
}
DB::DB() {
    kvdb = new std::unordered_map<string,Key *> {};
    ssdb = new std::vector<string> {};
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
        std::cout << "MDB: recalc'ing strand for key: " << iter.first << "\n";
        _recalcOneStrand(iter.second);
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
    std::cout << "MDB: inserted hunk " << k << " = " << v << "\n";
}
void DB::insertSentenceStarter(string ss) {
    ssdb->insert(ssdb->end(), ss);
    std::cout << "MDB: inserted sentence starter " << ss << "\n";
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
Key *DB::_weightedRNG(Key *kp) {
    int wt_sum = 0;
    for (auto k : *kp) {
        std::cout << "MDB: weight for " << k->ptr->val << " is " << k->ptr->getWeight() << "\n";
        wt_sum += (k->ptr->getWeight());
    }
    int rand = std::rand() % wt_sum;
    for (auto k : *kp) {
        std::cout << "MDB: rand " << rand << " curk " << k->ptr->val << "\n";
        if (rand < (k->ptr->getWeight()))
            return kp;
        rand -= (k->ptr->getWeight());
    }
    throw std::runtime_error("Weighted random number generator derped");
}
string DB::build_sentence(void) {
    if (ssdb->size() == 0)
        throw std::invalid_argument("No sentence starters in database.");
    string startk = ssdb->at(std::rand() % ssdb->size());
    string ret = startk;
    _debugDB();
    std::cout << "MDB: got sentence starter: " << startk << "\n";
    auto iter = kvdb->find(startk);
    while (iter != kvdb->end()) {
        string str = _weightedRNG(iter->second)->val;
        std::cout << "MDB: str " << str << "\n";
        ret = ret + " " + str;
        iter = kvdb->find(str);
    }
    std::cout << "MDB: built sentence " << ret << "\n";
    return ret;
}
