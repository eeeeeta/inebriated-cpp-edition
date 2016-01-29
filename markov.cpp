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
/**
 * Iterator for Key.
 * Should be rather simple and self-explanatory.
 */
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
/**
 * Linked list, used as values in the Markov database.
 *
 * To be able to store both values (string of next key) and
 * attributes (weightings for better sentence selection), we use
 * this class.
 */
class Key {
public:
    using iterator = KeyIter<Key>;

    Key *next; /**< next Key in linked list */
    string val; /**< value string */
    int weight; /**< weighting, determined by amount of values */
    int cweight; /**< weighting, determined by amount of repeat insertions (frequency) */

    Key(string istr) {
        val = istr;
        weight = cweight = 0;
        next = nullptr;
    }
/**
 * Destructor. Deletes next Key in linked list, if available.
 *
 * This has a rather neat 'cascading destruction' effect: this calls
 * `delete` on `next`, which activates its constructor, which calls `delete`
 * on its `next`...
 */
    ~Key() {
        if (next != nullptr) delete next;
    }
/**
 * Grow the linked list, adding value `istr`.
 * @param istr Value to add to list of values.
 */
    Key *grow(string istr) {
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
/**
 * Returns an iterator to the nth item in the linked list.
 * @param n item to look up
 */
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
    std::unordered_map<string,Key *>* kvdb; /**< string to Key map */
    std::vector<string> *ssdb; /**< sentence-starter database */

/**
 * Recalculate weightings for one 'strand' (linked list) of Key objects.
 */
    void _recalcOneStrand(Key *strand) {
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
/**
 * Recalculate all database weightings.
 */
    void recalcWeights(void) {
        for (auto iter : *kvdb) {
            std::cout << "MDB: recalc'ing strand for key: " << iter.first << "\n";
            _recalcOneStrand(iter.second);
        }
    }
/**
 * Insert a key-value pair into the database.
 * @param k key to insert
 * @param v value to insert
 */
    void insertHunk(string k, string v) {
        auto iter = kvdb->find(k);
        if (iter != kvdb->end()) {
            (iter->second)->grow(v);
        }
        else {
            kvdb->insert({k, new Key {v}});
        }
        std::cout << "MDB: inserted hunk " << k << " = " << v << "\n";
        recalcWeights();
    }
/**
 * Insert a string as a "sentence starter".
 * This helps to start sentences at the right place, instead
 * of getting a truncated half-sentence.
 * @param ss string to insert
 */
    void insertSentenceStarter(string ss) {
        ssdb->insert(ssdb->end(), ss);
        std::cout << "MDB: inserted sentence starter " << ss << "\n";
    }
/**
 * Build a sentence!
 * @exception std::invalid_argument No sentence starters exist to build a sentence from.
 */
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
        std::string tok {""}, ctok {""}, temp;
        int spaces = 0;
        while (std::getline(ss, temp, ' ')) {
            ctok = (ctok == "" ? temp : ctok + " " + temp);
            if (++spaces == 2) {
                if (tok == "") {
                    mdb.insertSentenceStarter(ctok);
                }
                else {
                    mdb.insertHunk(tok, ctok);
                }
                tok = ctok;
                spaces = 0;
                ctok = "";
            }
        }
        if (ctok != "") mdb.insertHunk(tok, ctok);
    }
    std::cin.clear();
    std::cout << "\nSentence input complete.\nGenerating sentences (Enter for new, Control-D to stop)\n\n";
    while (!std::cin.eof()) {
        mdb.build_sentence();
        string str;
        std::getline(std::cin, str);
    };
};
