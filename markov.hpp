#ifndef MARKOV_HPP
#define MARKOV_HPP
#include <string>
#include <unordered_map>
#include <vector>
namespace Markov {
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

        Key(string istr);
/**
 * Destructor. Deletes next Key in linked list, if available.
 *
 * This has a rather neat 'cascading destruction' effect: this calls
 * `delete` on `next`, which activates its constructor, which calls `delete`
 * on its `next`...
 */
        ~Key();
/**
 * Grow the linked list, adding value `istr`.
 * @param istr Value to add to list of values.
 */
        Key *grow(string istr);
        int getWeight(void);
        int length(void);
/**
 * Returns an iterator to the nth item in the linked list.
 * @param n item to look up
 */
        iterator advance(int n);
        iterator begin(void);
        iterator end(void);
    };
/**
 * The markov database itself - stores sentences and a list of values for keys.
 */
    class DB {
public:
        using kvdb_type = std::unordered_map<string, Key *>;
        using ssdb_objtype = std::pair<string, int>;
private:
        void _recalcOneStrand(Key *);
        void _debugDB(void);
        Key *_weightedRNG(Key *);
        kvdb_type* kvdb; /**< string to Key map */
        std::vector<std::pair<string, int>> *ssdb; /**< sentence-starter database */

public:
        DB();
        ~DB();
/**
 * Insert a key-value pair into the database.
 * @param k key to insert
 * @param v value to insert
 */
        void insertHunk(string k, string v);
/**
 * Insert a string as a "sentence starter".
 * This helps to start sentences at the right place, instead
 * of getting a truncated half-sentence.
 * @param ss string to insert
 */
        void insertSentenceStarter(string ss);
/**
 * Build a sentence!
 * @exception std::invalid_argument No sentence starters exist to build a sentence from.
 */
        string build_sentence(void);
        void recalcWeights(void);
        void doSavingThings(void);
        void doReadingThings(void);
    };
}
#endif
