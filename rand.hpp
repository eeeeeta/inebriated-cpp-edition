#ifndef MARKOV_RAND_HPP
#define MARKOV_RAND_HPP
#include "markov.hpp"
namespace Rand {
/**
 * Picks a random Markov::Key from a given linked list.
 * Will call `Key.getWeight()` on each Markov::Key to determine weightings.
 * @param ll first Markov::Key in linked list
 */
    Markov::Key *randomKey(Markov::Key *ll);
/**
 * Evaluates, and returns, the weighting for picking this list.
 */
    float weightForKey(Markov::Key *ll, Markov::DB::kvdb_type *kvdb);
    Markov::DB::ssdb_objtype randomSsdbObj(std::vector<Markov::DB::ssdb_objtype>* vec);
};
#endif
