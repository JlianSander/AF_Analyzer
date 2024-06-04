#ifndef VECTOR_BITSET_H
#define VECTOR_BITSET_H

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iterator>

using namespace std;

class VectorBitSet {

public:

    std::vector<uint32_t> _vector;
    std::vector<uint8_t> _bitset;

    VectorBitSet() {}

    VectorBitSet(std::vector<uint32_t> new_vector, std::vector<uint8_t> new_bitset) : _vector{ new_vector }, _bitset{ new_bitset } {}

    ~VectorBitSet() {
        _bitset.clear();
        _vector.clear();
    }

    VectorBitSet copy() {
        const uint64_t vector_size = _vector.size();
        std::vector<uint32_t> copy_vector;
        const uint64_t bitset_size = _bitset.size();
        std::vector<uint8_t> copy_bitset;
        copy_bitset.resize(bitset_size);

        for (int i = 0; i < bitset_size; i++) {
            if (i < vector_size) {
                copy_vector.push_back(_vector[i]);
            }

            copy_bitset[i] = _bitset[i];
        }

        return VectorBitSet(copy_vector, copy_bitset);
    }

    void clear() {
        _bitset.clear();
        _vector.clear();
    }
};

#endif