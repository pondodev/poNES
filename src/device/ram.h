#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include <stdlib.h>

int ram_read(uint16_t addr, void* out, size_t n);
int ram_write(uint16_t addr, const void* in, size_t n);

#endif

