#include "memory.h"

#include "raylib.h"

#include <time.h>
#include <string.h>

// memory map from https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_START          0x0000
#define INTERNAL_RAM_END            0x07FF
#define INTERNAL_RAM_SIZE           0x0800

#define INTERNAL_RAM_MIRROR_START   0x0800 // mirrors every 2KB (3 times)
#define INTERNAL_RAM_MIRROR_END     0x1FFF
#define INTERNAL_RAM_MIRROR_SIZE    0x1800

#define PPU_REG_START               0x2000
#define PPU_REG_END                 0x2007
#define PPU_REG_SIZE                0x0008

#define PPU_REG_MIRROR_START        0x2008 // mirrors every 8B (1023 times)
#define PPU_REG_MIRROR_END          0x3FFF
#define PPU_REG_MIRROR_SIZE         0x1FF8

#define APU_IO_REG_START            0x4000
#define APU_IO_REG_END              0x4017
#define APU_IO_REG_SIZE             0x0018

#define APU_IO_FUNC_START           0x4018
#define APU_IO_FUNC_END             0x401F
#define APU_IO_FUNC_SIZE            0x0008

// NOTE: unmapped area can (and usually does) contain cart RAM and ROM
#define UNMAPPED_START              0x4020
#define UNMAPPED_END                0xFFFF
#define UNMAPPED_SIZE               0xBFE0

#define CART_ROM_BANK_START         0x8000
#define CART_ROM_BANK_END           0xFFFF
#define CART_ROM_BANK_SIZE          0x8000

static uint8_t s_internal_ram[INTERNAL_RAM_SIZE];
static uint8_t s_ppu_reg[PPU_REG_SIZE];
static uint8_t s_apu_io_reg[APU_IO_REG_SIZE];
static uint8_t s_apu_io_func[APU_IO_FUNC_SIZE];
static uint8_t s_unmapped[UNMAPPED_SIZE];

static inline void _randomise_buffer(uint8_t* buffer, size_t n);
static inline int _transform_addr(uint16_t addr, uint16_t* offset_out, size_t read_size, size_t region_start, size_t region_end);
static inline void _read_mem(uint8_t* mem, size_t offset, void* out, size_t n);
static inline void _write_mem(uint8_t* mem, size_t offset, const void* in, size_t n);

static inline int _read_internal_ram(uint16_t addr, void* out, size_t n);
static inline int _read_ppu_reg(uint16_t addr, void* out, size_t n);
static inline int _read_apu_io_reg(uint16_t addr, void* out, size_t n);
static inline int _read_apu_io_func(uint16_t addr, void* out, size_t n);
static inline int _read_unmapped(uint16_t addr, void* out, size_t n);
static inline int _write_internal_ram(uint16_t addr, const void* in, size_t n);
static inline int _write_ppu_reg(uint16_t addr, const void* in, size_t n);
static inline int _write_apu_io_reg(uint16_t addr, const void* in, size_t n);
static inline int _write_apu_io_func(uint16_t addr, const void* in, size_t n);
static inline int _write_unmapped(uint16_t addr, const void* in, size_t n);

void memory_init(void) {
    _randomise_buffer(s_internal_ram, INTERNAL_RAM_SIZE);
    _randomise_buffer(s_ppu_reg, PPU_REG_SIZE);
    _randomise_buffer(s_apu_io_reg, APU_IO_REG_SIZE);
    _randomise_buffer(s_apu_io_func, APU_IO_FUNC_SIZE);
    _randomise_buffer(s_unmapped, UNMAPPED_SIZE);
}

void memory_read(uint16_t addr, void* out, size_t n) {
    const int success = _read_internal_ram(addr, out, n) ||
                        _read_ppu_reg(addr, out, n) ||
                        _read_apu_io_reg(addr, out, n) ||
                        _read_apu_io_func(addr, out, n) ||
                        _read_unmapped(addr, out, n);

    if (! success)
        TraceLog(LOG_ERROR, "failed to read %zu byte(s) of memory at address 0x%04X\n", n, addr);
}

void memory_write(uint16_t addr, const void* in, size_t n) {
    const int success = _write_internal_ram(addr, in, n) ||
                        _write_ppu_reg(addr, in, n) ||
                        _write_apu_io_reg(addr, in, n) ||
                        _write_apu_io_func(addr, in, n) ||
                        _write_unmapped(addr, in, n);

    if (! success)
        TraceLog(LOG_ERROR, "failed to write %zu byte(s) of memory at address 0x%04X\n", n, addr);
}

uint8_t memory_read8(uint16_t addr) {
    uint8_t out;
    memory_read(addr, &out, sizeof(out));

    return out;
}

void memory_write8(uint16_t addr, uint8_t data) {
    memory_write(addr, &data, sizeof(data));
}

void memory_load_cart_rom_bank(const void* buffer, size_t n) {
    if (n > CART_ROM_BANK_SIZE) {
        TraceLog(LOG_ERROR, "attempted to load cart ROM of size %zu. max size is %zu", n, CART_ROM_BANK_SIZE);
        return;
    }

    _write_unmapped(CART_ROM_BANK_START, buffer, n);
}

static inline void _randomise_buffer(uint8_t* buffer, size_t n) {
    static int rand_seeded = 0;
    if (! rand_seeded) {
        srand(time(NULL));
        rand_seeded = 1;
    }

    for (size_t i = 0; i < n; ++i)
        buffer[i] = rand() % 0xFF;
}

static inline int _transform_addr(uint16_t addr, uint16_t* offset_out, size_t read_size, size_t region_start, size_t region_end) {
    const int addr_out_of_bounds = addr < region_start || addr > region_end;
    const int read_out_of_bounds = ((size_t)addr + read_size) > region_end;
    if (addr_out_of_bounds || read_out_of_bounds)
        return 0;

    *offset_out = addr - region_start;
    return 1;
}

static inline void _read_mem(uint8_t* mem, size_t offset, void* out, size_t n) {
    memcpy(out, mem + offset, n);
}

static inline void _write_mem(uint8_t* mem, size_t offset, const void* in, size_t n) {
    memcpy(mem + offset, in, n);
}

static inline int _read_internal_ram(uint16_t addr, void* out, size_t n) {
    uint16_t offset;
    // check if we're trying to read from a mirror instead
    if (_transform_addr(addr, &offset, n, INTERNAL_RAM_MIRROR_START, INTERNAL_RAM_MIRROR_END)) {
        // transform the offset to an index into s_internal_ram
        offset %= INTERNAL_RAM_SIZE;
    } else if (! _transform_addr(addr, &offset, n, INTERNAL_RAM_START, INTERNAL_RAM_END)) {
        return 0;
    }

    _read_mem(s_internal_ram, offset, out, n);
    return 1;
}

static inline int _read_ppu_reg(uint16_t addr, void* out, size_t n) {
    uint16_t offset;
    // check if we're trying to read from a mirror instead
    if (_transform_addr(addr, &offset, n, PPU_REG_MIRROR_START, PPU_REG_MIRROR_END)) {
        // transform the offset to an index into s_ppu_reg
        offset %= PPU_REG_SIZE;
    } else if (! _transform_addr(addr, &offset, n, PPU_REG_START, PPU_REG_END)) {
        return 0;
    }

    _read_mem(s_ppu_reg, offset, out, n);
    return 1;
}

static inline int _read_apu_io_reg(uint16_t addr, void* out, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, APU_IO_REG_START, APU_IO_REG_END))
        return 0;

    _read_mem(s_apu_io_reg, offset, out, n);
    return 1;
}

static inline int _read_apu_io_func(uint16_t addr, void* out, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, APU_IO_FUNC_START, APU_IO_FUNC_END))
        return 0;

    _read_mem(s_apu_io_func, offset, out, n);
    return 1;
}

static inline int _read_unmapped(uint16_t addr, void* out, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, UNMAPPED_START, UNMAPPED_END))
        return 0;

    _read_mem(s_unmapped, offset, out, n);
    return 1;
}

static inline int _write_internal_ram(uint16_t addr, const void* in, size_t n) {
    uint16_t offset;
    // check if we're trying to write to a mirror instead
    if (_transform_addr(addr, &offset, n, INTERNAL_RAM_MIRROR_START, INTERNAL_RAM_MIRROR_END)) {
        // transform the offset to an index into s_internal_ram
        offset %= INTERNAL_RAM_SIZE;
    } else if (! _transform_addr(addr, &offset, n, INTERNAL_RAM_START, INTERNAL_RAM_END)) {
        return 0;
    }

    _write_mem(s_internal_ram, offset, in, n);
    return 1;
}

static inline int _write_ppu_reg(uint16_t addr, const void* in, size_t n) {
    uint16_t offset;
    // check if we're trying to write to a mirror instead
    if (_transform_addr(addr, &offset, n, PPU_REG_MIRROR_START, PPU_REG_MIRROR_END)) {
        // transform the offset to an index into s_ppu_reg
        offset %= PPU_REG_SIZE;
    } else if (! _transform_addr(addr, &offset, n, PPU_REG_START, PPU_REG_END)) {
        return 0;
    }

    _write_mem(s_ppu_reg, offset, in, n);
    return 1;
}

static inline int _write_apu_io_reg(uint16_t addr, const void* in, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, APU_IO_REG_START, APU_IO_REG_END))
        return 0;

    _write_mem(s_apu_io_reg, offset, in, n);
    return 1;
}

static inline int _write_apu_io_func(uint16_t addr, const void* in, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, APU_IO_FUNC_START, APU_IO_FUNC_END))
        return 0;

    _write_mem(s_apu_io_func, offset, in, n);
    return 1;
}

static inline int _write_unmapped(uint16_t addr, const void* in, size_t n) {
    uint16_t offset;
    if (! _transform_addr(addr, &offset, n, UNMAPPED_START, UNMAPPED_END))
        return 0;

    _write_mem(s_unmapped, offset, in, n);
    return 1;
}

