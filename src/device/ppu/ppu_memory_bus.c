#include "ppu_memory_bus.h"

#include "ppu_memory_map.h"

#include "log.h"

typedef enum {
    kPPU_BUS_LOCATION_PATTERN_TABLE_0,
    kPPU_BUS_LOCATION_PATTERN_TABLE_1,
    kPPU_BUS_LOCATION_NAMETABLE_0,
    kPPU_BUS_LOCATION_NAMETABLE_1,
    kPPU_BUS_LOCATION_NAMETABLE_2,
    kPPU_BUS_LOCATION_NAMETABLE_3,
    kPPU_BUS_LOCATION_UNUSED,
    kPPU_BUS_LOCATION_PALETTE_RAM,

    kPPU_BUS_LOCATION_UNKNOWN,
} PPUBusLocation;

static inline PPUBusLocation _get_ppu_bus_location(uint16_t addr);

int ppu_memory_bus_read(uint16_t addr, void* out, size_t n) {
    if ((PPU_MEMORY_SIZE - n) < addr) {
        log_error("attempted to read %zu byte(s) from address 0x%04X on the PPU bus, which would be out of bounds", n, addr);
        return 0;
    }

    uint8_t* buf = (uint8_t*)out;
    for (size_t i = 0; i < n; ++i) {
        const PPUBusLocation location = _get_ppu_bus_location(addr);
        switch (location) {
            // TODO
            case kPPU_BUS_LOCATION_PATTERN_TABLE_0: return 0;
            case kPPU_BUS_LOCATION_PATTERN_TABLE_1: return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_0:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_1:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_2:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_3:     return 0;
            case kPPU_BUS_LOCATION_UNUSED:
            {
                buf[i] = 0;
                return 1;
            }
            case kPPU_BUS_LOCATION_PALETTE_RAM:     return 0;

            default:
                log_error("attempted to read from memory not mapped in the PPU bus (0x%04X)", addr);
                break;
        }
    }

    return 0;
}

int ppu_memory_bus_write(uint16_t addr, const void* in, size_t n) {
    if ((PPU_MEMORY_SIZE - n) < addr) {
        log_error("attempted to write %zu byte(s) to address 0x%04X on the PPU bus, which would be out of bounds", n, addr);
        return 0;
    }

    const uint8_t* buf = (uint8_t*)in;
    for (size_t i = 0; i < n; ++i) {
        const PPUBusLocation location = _get_ppu_bus_location(addr);
        switch (location) {
            // TODO
            case kPPU_BUS_LOCATION_PATTERN_TABLE_0: return 0;
            case kPPU_BUS_LOCATION_PATTERN_TABLE_1: return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_0:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_1:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_2:     return 0;
            case kPPU_BUS_LOCATION_NAMETABLE_3:     return 0;
            case kPPU_BUS_LOCATION_UNUSED:          return 1;
            case kPPU_BUS_LOCATION_PALETTE_RAM:     return 0;

            default:
                log_error("attempted to write to memory not mapped in the PPU bus (0x%04X)", addr);
                break;
        }
    }

    return 0;
}

static inline PPUBusLocation _get_ppu_bus_location(uint16_t addr) {
    if (addr >= PATTERN_TABLE_0_START && addr <= PATTERN_TABLE_0_END)
        return kPPU_BUS_LOCATION_PATTERN_TABLE_0;
    if (addr >= PATTERN_TABLE_1_START && addr <= PATTERN_TABLE_1_END)
        return kPPU_BUS_LOCATION_PATTERN_TABLE_1;
    if (addr >= NAMETABLE_0_START && addr <= NAMETABLE_0_END)
        return kPPU_BUS_LOCATION_NAMETABLE_0;
    if (addr >= NAMETABLE_1_START && addr <= NAMETABLE_1_END)
        return kPPU_BUS_LOCATION_NAMETABLE_1;
    if (addr >= NAMETABLE_2_START && addr <= NAMETABLE_2_END)
        return kPPU_BUS_LOCATION_NAMETABLE_2;
    if (addr >= NAMETABLE_3_START && addr <= NAMETABLE_3_END)
        return kPPU_BUS_LOCATION_NAMETABLE_3;
    if (addr >= UNUSED_START && addr <= UNUSED_END)
        return kPPU_BUS_LOCATION_UNUSED;
    if (addr >= PALETTE_RAM_INDICES_START && addr <= PALETTE_RAM_INDICES_END)
        return kPPU_BUS_LOCATION_PALETTE_RAM;
    if (addr >= PALETTE_RAM_INDICES_MIRROR_START && addr <= PALETTE_RAM_INDICES_MIRROR_END)
        return kPPU_BUS_LOCATION_PALETTE_RAM;

    return kPPU_BUS_LOCATION_UNKNOWN;
}
