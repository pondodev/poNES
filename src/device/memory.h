#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdlib.h>

void memory_init(void);
void memory_read(uint16_t addr, void* out, size_t n);
void memory_write(uint16_t addr, const void* in, size_t n);
uint8_t memory_read8(uint16_t addr);
void memory_write8(uint16_t addr, uint8_t byte);
void memory_load_cart_rom_bank(const void* buffer, size_t n);

#endif

