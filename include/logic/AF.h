#ifndef AF_H
#define AF_H

#include <vector>
#include <unordered_set>
#include <cstdint>
#include <iostream>	
#include <omp.h>

#include "../util/VectorBitSet.h"

template <class T>
inline void hash_combine(std::size_t &seed, const T &v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
	template<typename S, typename T> struct hash<pair<S, T>>
	{
		inline size_t operator()(const pair<S, T> &v) const
		{
			size_t seed = 0;
			::hash_combine(seed, v.first);
			::hash_combine(seed, v.second);
			return seed;
		}
	};
}

class AF {

public:

    uint32_t num_args;
    std::vector<VectorBitSet> attackers;
	std::vector<VectorBitSet> victims;
	std::vector<uint8_t> self_attack;
    std::unordered_set<std::pair<uint32_t, uint32_t>> attacks;
	std::unordered_set<std::pair<uint32_t, uint32_t>> symmetric_attacks;
	std::vector<uint32_t> distance_to_query;

    bool add_attack(uint32_t attacker, uint32_t victim);
	void initialize(uint32_t number_args);
    void finish_initilization();
    bool exists_attack(uint32_t attacker, uint32_t victim) const;
};

#endif