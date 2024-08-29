#include "helpers.h"

void set_bit(uint8_t* bitset, uint8_t idx) {
    *bitset |= 1 << idx;
}

void unset_bit(uint8_t* bitset, uint8_t idx) {
    *bitset &= ~(1 << idx);
}

int read_bit(uint8_t bitset, uint8_t idx) {
    return (bitset >> idx) & 1;
}

