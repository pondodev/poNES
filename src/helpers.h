#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <stdlib.h>

#define BIT(n) (1 << n)

void set_bit(uint8_t* bitset, uint8_t idx);
void unset_bit(uint8_t* bitset, uint8_t idx);
int read_bit(uint8_t bitset, uint8_t idx);

void randomise_buffer(void* buf, size_t n);

#endif

