#include "ram.h"

#include "memory_map.h"
#include "helpers.h"

static uint8_t s_ram[INTERNAL_RAM_SIZE];

static inline uint16_t _transform_addr(uint16_t addr);

void ram_init(void) {
    randomise_buffer(s_ram, INTERNAL_RAM_SIZE);
}

int ram_read8(uint16_t addr, uint8_t* out) {
    const uint16_t ram_idx  = _transform_addr(addr);
    *out = s_ram[ram_idx];

    return 1;
}

int ram_write8(uint16_t addr, const uint8_t* in) {
    const uint16_t ram_idx  = _transform_addr(addr);
    s_ram[ram_idx]          = *in;

    return 1;
}

static inline uint16_t _transform_addr(uint16_t addr) {
    addr -= INTERNAL_RAM_START;
    return addr % INTERNAL_RAM_SIZE;
}

