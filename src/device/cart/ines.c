#include "ines.h"

#include "log.h"

#define HEADER_SIZE_BYTES 16
#define TRAINER_SIZE_BYTES 512
#define PRG_ROM_SIZE_MULTIPLIER 16 * 1024
#define CHR_ROM_SIZE_MULTIPLIER 8 * 1024

INESHeader* ines_load(const uint8_t* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        log_error("buffer can't be empty");
        return NULL;
    }

    if (size < 16) {
        log_error("buffer is too small");
        return NULL;
    }

    INESHeader* header = malloc(sizeof(INESHeader));

    header->prg_rom_blocks  = buffer[4];
    header->chr_rom_blocks  = buffer[5];
    header->flags_6         = buffer[6];
    header->flags_7         = buffer[7];
    header->flags_8         = buffer[8];
    header->flags_9         = buffer[9];
    header->flags_10        = buffer[10];

    return header;
}

void ines_unload(INESHeader* header) {
    free(header);
}

size_t ines_prg_rom_size_bytes(const INESHeader* header) {
    return header->prg_rom_blocks * PRG_ROM_SIZE_MULTIPLIER;
}

uint16_t ines_prg_rom_start(const INESHeader* header) {
    const uint16_t base_addr    = HEADER_SIZE_BYTES;
    const size_t trainer_size   = ines_has_trainer(header) ? TRAINER_SIZE_BYTES : 0;

    return base_addr + trainer_size;
}

size_t ines_chr_rom_size_bytes(const INESHeader* header) {
    return header->chr_rom_blocks * CHR_ROM_SIZE_MULTIPLIER;
}

uint16_t ines_chr_rom_start(const INESHeader* header) {
    const int has_chr_rom = ines_chr_rom_size_bytes(header) != 0;
    if (! has_chr_rom)
        return 0;

    return ines_prg_rom_start(header) + ines_prg_rom_size_bytes(header);
}

INESNametableArrangement ines_nametable_arrangement(const INESHeader* header) {
    const int arrangement = header->flags_6 & 0x01;
    return arrangement ? kINESNametableArrangement_Horizontal : kINESNametableArrangement_Vertical;
}

int ines_has_prg_ram(const INESHeader* header) {
    return header->flags_6 & (0x01 << 1);
}

int ines_has_trainer(const INESHeader* header) {
    return header->flags_6 & (0x01 << 2);
}

uint8_t ines_mapper(const INESHeader* header) {
    const uint8_t lower = (header->flags_6 & 0xF0) >> 4;
    const uint8_t upper = header->flags_7 & 0xF0;

    return lower & upper;
}

