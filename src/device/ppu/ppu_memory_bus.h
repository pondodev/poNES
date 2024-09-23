#ifndef PPU_MEMORY_BUS_H
#define PPU_MEMORY_BUS_H

#include <stdint.h>
#include <stdlib.h>

int ppu_memory_bus_read(uint16_t addr, void* out, size_t n);
int ppu_memory_bus_write(uint16_t addr, const void* in, size_t n);

#endif

