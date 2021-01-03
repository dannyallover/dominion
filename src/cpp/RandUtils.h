/* DOMINION
 * David Mally, Richard Roberts
 * RandUtils.h
 * Defines a function to generate a pseudorandom list of numbers.
 * Used as a fallback when we run out of API requests or are offline.
 */
#ifndef __RANDUTILS_H__
#define __RANDUTILS_H__

#include <vector>
#include <cstdlib>

namespace rand_utils {
    std::vector<int> GenPseudoRandList(size_t size, int max, int seed);
}

#endif
