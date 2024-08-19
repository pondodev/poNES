#ifndef INES_H
#define INES_H

#include <stdlib.h>

typedef struct {
    uint8_t prg_rom_blocks;
    uint8_t chr_rom_blocks;
    uint8_t flags_6;
    uint8_t flags_7;
    uint8_t flags_8;
    uint8_t flags_9;
    uint8_t flags_10;
} INESHeader;

typedef enum {
    kINESNametableArrangement_Horizontal,
    kINESNametableArrangement_Vertical,
} INESNametableArrangement;

INESHeader* ines_load(const uint8_t* buffer, size_t size);
void ines_unload(INESHeader* header);

size_t ines_prg_rom_size_bytes(const INESHeader* header);
uint16_t ines_prg_rom_start(const INESHeader* header);
size_t ines_chr_rom_size_bytes(const INESHeader* header);
uint16_t ines_chr_rom_start(const INESHeader* header);
INESNametableArrangement ines_nametable_arrangement(const INESHeader* header);
int ines_has_prg_ram(const INESHeader* header);
int ines_has_trainer(const INESHeader* header);
uint8_t ines_mapper(const INESHeader* header);

#endif

