#ifndef MEMORY_BUS_H
#define MEMORY_BUS_H

#include <stdint.h>
#include <stdlib.h>

int memory_bus_read(uint16_t addr, void* out, size_t n);
int memory_bus_write(uint16_t addr, const void* in, size_t n);

#endif

