#ifndef CART_H
#define CART_H

#include <stdlib.h>

typedef enum {
    kROMFORMAT_NONE = 0,
    kROMFORMAT_INES,
    kROMFORMAT_INES20,
} ROMFormat;

typedef struct {
    ROMFormat               format;
    void*                   format_header;
    uint8_t*                buffer;
    size_t                  buffer_size;
    uint16_t                prg_rom_start;
    size_t                  prg_rom_size;
    uint16_t                chr_rom_start;
    size_t                  chr_rom_size;
    size_t                  prg_ram_size;
} Cart;

int cart_load(const char* path, Cart* cart);
void cart_unload(Cart* cart);

#endif

