#include "helpers.h"

#include <time.h>

void set_bit(uint8_t* bitset, uint8_t idx) {
    *bitset |= 1 << idx;
}

void unset_bit(uint8_t* bitset, uint8_t idx) {
    *bitset &= ~(1 << idx);
}

int read_bit(uint8_t bitset, uint8_t idx) {
    return (bitset >> idx) & 1;
}

void randomise_buffer(void* buf, size_t n) {
    if (n == 0)
        return;

    static int init = 0;
    if (! init) {
        srand(time(NULL));
        init = 1;
    }

    uint8_t* bytes = (uint8_t*)buf;
    for (size_t i = 0; i < n; ++i)
        bytes[i] = rand();
}

