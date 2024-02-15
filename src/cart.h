#ifndef CART_H
#define CART_H

#include <stdlib.h>

typedef enum {
    kROMTYPE_NONE = 0,
    kROMTYPE_INES,
    kROMTYPE_INES20,
} ROMType;

typedef enum {
    kNAMETABLE_VERTICAL,
    kNAMETABLE_HORIZONTAL,
} NametableArrangement;

typedef struct {
    ROMType                 type;
    size_t                  size;
    uint8_t*                buffer;
    size_t                  prg_rom_size;
    size_t                  chr_rom_size;
    size_t                  prg_ram_size;
    NametableArrangement    nametable_arrangement;
    uint16_t                mapper;
    uint16_t                start_addr;
} Cart;

int cart_load(const char* path, Cart* cart);
void cart_unload(Cart* cart);

#endif

