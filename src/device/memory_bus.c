#include "memory_bus.h"

#include "memory_map.h"
#include "ram.h"
#include "ppu.h"
#include "cpu.h"
#include "cart/cart.h"

#include "log.h"

typedef enum {
    kBUS_LOCATION_RAM,
    kBUS_LOCATION_PPU_REG,
    kBUS_LOCATION_APU_IO_REG,
    kBUS_LOCATION_UNMAPPED,

    kBUS_LOCATION_UNKNOWN,
} BusLocation;

static inline BusLocation _get_bus_location(uint16_t addr);

int memory_bus_read(uint16_t addr, void* out, size_t n) {
    if ((MEMORY_SIZE - n) < addr) {
        log_error("attempted to read %zu byte(s) from address 0x%04X, which would be out of bounds", n, addr);
        return 0;
    }

    const BusLocation location = _get_bus_location(addr);
    switch (location)
    {
        case kBUS_LOCATION_RAM:         return ram_read(addr, out, n);
        case kBUS_LOCATION_PPU_REG:     return ppu_reg_read(addr, out, n);
        case kBUS_LOCATION_APU_IO_REG:  return cpu_apu_io_reg_read(addr, out, n);
        case kBUS_LOCATION_UNMAPPED:    return cart_read(addr, out, n);

        default:
            log_error("attempted to read from memory not mapped in the bus (0x%04X)", addr);
            break;
    }

    return 0;
}

int memory_bus_write(uint16_t addr, const void* in, size_t n) {
    if ((MEMORY_SIZE - n) < addr) {
        log_error("attempted to write %zu byte(s) to address 0x%04X, which would be out of bounds", n, addr);
        return 0;
    }

    const BusLocation location = _get_bus_location(addr);
    switch (location)
    {
        case kBUS_LOCATION_RAM:         return ram_write(addr, in, n);
        case kBUS_LOCATION_PPU_REG:     return ppu_reg_write(addr, in, n);
        case kBUS_LOCATION_APU_IO_REG:  return cpu_apu_io_reg_write(addr, in, n);
        case kBUS_LOCATION_UNMAPPED:    return cart_write(addr, in, n);

        default:
            log_error("attempted to read from memory not mapped in the bus (0x%04X)", addr);
            break;
    }

    return 0;
}

static inline BusLocation _get_bus_location(uint16_t addr) {
    if (addr >= INTERNAL_RAM_START && addr <= INTERNAL_RAM_END)
        return kBUS_LOCATION_RAM;
    if (addr >= INTERNAL_RAM_MIRROR_START && addr <= INTERNAL_RAM_MIRROR_END)
        return kBUS_LOCATION_RAM;
    if (addr >= PPU_REG_START && addr <= PPU_REG_END)
        return kBUS_LOCATION_PPU_REG;
    if (addr >= PPU_REG_MIRROR_START && addr <= PPU_REG_MIRROR_END)
        return kBUS_LOCATION_PPU_REG;
    if (addr >= APU_IO_REG_START && addr <= APU_IO_REG_END)
        return kBUS_LOCATION_APU_IO_REG;
    if (addr >= UNMAPPED_START && addr <= UNMAPPED_END)
        return kBUS_LOCATION_UNMAPPED;

    return kBUS_LOCATION_UNKNOWN;
}

