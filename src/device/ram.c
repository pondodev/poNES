#include "ram.h"

#include "memory_map.h"
#include "helpers.h"

static uint8_t s_ram[INTERNAL_RAM_SIZE];

static inline uint16_t _transform_addr(uint16_t addr);

void ram_init(void) {
    randomise_buffer(s_ram, INTERNAL_RAM_SIZE);
}

int ram_read(uint16_t addr, void* out, size_t n) {
    uint8_t* buf = (uint8_t*)out;

    for (size_t i = 0; i < n; ++i) {
        const uint16_t ram_idx = _transform_addr(addr+i);
        buf[i] = s_ram[ram_idx];
    }

    return 1;
}

int ram_write(uint16_t addr, const void* in, size_t n) {
    const uint8_t* buf = (const uint8_t*)in;

    for (size_t i = 0; i < n; ++i) {
        const uint16_t ram_idx = _transform_addr(addr+i);
        s_ram[ram_idx] = buf[i];
    }

    return 1;
}

static inline uint16_t _transform_addr(uint16_t addr) {
    addr -= INTERNAL_RAM_START;
    return addr % INTERNAL_RAM_SIZE;
}

