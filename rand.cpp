/*
 * Random number generation, weighting, and all that good stuff(tm)
 */
#include "rand.hpp"
#include <iostream>
using namespace Markov;

Key *Rand::randomKey(Key *kp) {
    int wt_sum = 0;
    for (auto k : *kp) {
        std::cout << "MDB: weight for " << k->ptr->val << " is " << k->ptr->getWeight() << "\n";
        wt_sum += (k->ptr->getWeight());
    }
    int rand = (wt_sum > 0 ? std::rand() % wt_sum : -1);
    for (auto k : *kp) {
        std::cout << "MDB: rand " << rand << " curk " << k->ptr->val << "\n";
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
        std::cout << "RAND: keylength " << key->length() << "\n";
        weight_so_far += ((float) key->length());
/* vk = choose random key from current linked list */
        Key *vk = Rand::randomKey(key);
        auto iter = kvdb->find(vk->val);
        if (iter == kvdb->end()) break;
        else key = iter->second;
/* if we found one, continue */
    }
    std::cout << "RAND: wt = " << weight_so_far << "\n";
    return weight_so_far;
};
