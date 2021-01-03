/* DOMINION
 * David Mally, Richard Roberts
 * RandUtils.cpp
 * Defines a function to generate a pseudorandom list of numbers.
 * Used as a fallback when we run out of API requests or are offline.
 */
#include <cstdlib>
#include <vector>
#include "RandUtils.h"

std::vector<int> rand_utils::GenPseudoRandList(size_t size, int max, int seed) {
    std::vector<int> pseudoRands;
    srand(seed);
    for(int i = 0; i < max; i++) {
        int randIdx = rand() % size;
        bool notUniq = false;
        do {
            notUniq = false;
            for(int j = 0; j < i; j++) {
                if(pseudoRands.at(j) == randIdx) {
                    notUniq = true;
                }
            }
            if(notUniq) {
                randIdx = rand() % size;
            }
        } while(notUniq);
        pseudoRands.push_back(randIdx);
    }
    return pseudoRands;
}
