#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#define MEMORY_SIZE                 0xFFFF

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

#endif

