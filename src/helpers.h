#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>

void set_bit(uint8_t* bitset, uint8_t idx);
void unset_bit(uint8_t* bitset, uint8_t idx);
int read_bit(uint8_t bitset, uint8_t idx);

#endif

