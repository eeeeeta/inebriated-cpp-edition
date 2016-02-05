/*
 * Random number generation, weighting, and all that good stuff(tm)
 */
#include "rand.hpp"
#include <iostream>
using namespace Markov;

Key *Rand::randomKey(Key *kp) {
    int wt_sum = 0;
    for (auto k : *kp) {
#ifdef DEBUG
        std::cout << "MDB: weight for " << k->ptr->val << " is " << k->ptr->getWeight() << "\n";
#endif
        wt_sum += (k->ptr->getWeight());
    }
    int rand = (wt_sum > 0 ? std::rand() % wt_sum : -1);
    for (auto k : *kp) {
#ifdef DEBUG
        std::cout << "MDB: rand " << rand << " curk " << k->ptr->val << "\n";
#endif
        if (rand < (k->ptr->getWeight())) {
            return k->ptr;
        }
        rand -= (k->ptr->getWeight());
    }
    throw std::runtime_error("Weighted random number generator derped");
};
DB::ssdb_objtype Rand::randomSsdbObj(std::vector<DB::ssdb_objtype>* vec) {
    int wt_sum = 0;
    for (auto obj : *vec)
        wt_sum += obj.second;
    int rand = (wt_sum > 0 ? std::rand() % wt_sum : -1);
    for (auto obj : *vec) {
        if (rand < obj.second)
            return obj;
        rand -= obj.second;
    }
    throw std::runtime_error("Weighted random number generator derped");
};
float Rand::weightForKey(Key *key, DB::kvdb_type *kvdb) {
    float weight_so_far = 0.0;
    for (;;) {
/* weight += amount of choices in current linked list - magical constant */
#ifdef DEBUG
        std::cout << "RAND: keylength " << key->length() << "\n";
#endif
        weight_so_far += ((float) key->length());
/* vk = choose random key from current linked list */
        Key *vk = Rand::randomKey(key);
        auto iter = kvdb->find(vk->val);
        if (iter == kvdb->end()) break;
        else key = iter->second;
/* if we found one, continue */
    }
#ifdef DEBUG
    std::cout << "RAND: wt = " << weight_so_far << "\n";
#endif
    return weight_so_far;
};
