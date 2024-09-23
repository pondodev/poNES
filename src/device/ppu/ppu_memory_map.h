#ifndef PPU_MEMORY_MAP_H
#define PPU_MEMORY_MAP_H

#define PPU_MEMORY_SIZE                     0x4000

// memory map from https://www.nesdev.org/wiki/PPU_memory_map
// NOTE: memory addressing for the PPU is 14-bit
#define PATTERN_TABLE_0_START               0x0000
#define PATTERN_TABLE_0_END                 0x0FFF
#define PATTERN_TABLE_0_SIZE                0x1000
#define PATTERN_TABLE_1_START               0x1000
#define PATTERN_TABLE_1_END                 0x1FFF
#define PATTERN_TABLE_1_SIZE                0x1000

#define NAMETABLE_0_START                   0x2000
#define NAMETABLE_0_END                     0x23BF
#define NAMETABLE_0_SIZE                    0x0400
#define NAMETABLE_1_START                   0x2400
#define NAMETABLE_1_END                     0x27FF
#define NAMETABLE_1_SIZE                    0x0400
#define NAMETABLE_2_START                   0x2800
#define NAMETABLE_2_END                     0x2BFF
#define NAMETABLE_2_SIZE                    0x0400
#define NAMETABLE_3_START                   0x2C00
#define NAMETABLE_3_END                     0x2FFF
#define NAMETABLE_3_SIZE                    0x0400

#define UNUSED_START                        0x3000
#define UNUSED_END                          0x3EFF
#define UNUSED_SIZE                         0x0F00

#define PALETTE_RAM_INDICES_START           0x3F00
#define PALETTE_RAM_INDICES_END             0x3F1F
#define PALETTE_RAM_INDICES_SIZE            0x0020

#define PALETTE_RAM_INDICES_MIRROR_START    0x3F20 // mirrors every 32B (7 times)
#define PALETTE_RAM_INDICES_MIRROR_END      0x3FFF
#define PALETTE_RAM_INDICES_MIRROR_SIZE     0x00E0

#endif

