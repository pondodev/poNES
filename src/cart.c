#include "cart.h"

#include "raylib.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

static inline int _parse_ines(Cart* cart);
static inline int _get_ines_start_addr(uint16_t mapper, uint16_t* start_addr_out);
static inline int _parse_ines20(Cart* cart);

int cart_load(const char* path, Cart* cart) {
    TraceLog(LOG_INFO, "loading cart from path '%s'...", path);

    int success = 1;

    if (cart == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (cart cannot be NULL)");
        return 0;
    }

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    fseek(f, 0, SEEK_END);
    cart->size = ftell(f);
    rewind(f);

    TraceLog(LOG_INFO, "reading %zu bytes...", cart->size);

    cart->buffer = malloc(cart->size);
    const size_t read_bytes = fread(cart->buffer, sizeof(cart->buffer[0]), cart->size, f);
    if (cart->size != read_bytes) {
        TraceLog(LOG_ERROR, "failed to load cart (expected %zu, got %zu)", cart->size, read_bytes);
        success = 0;
        goto bail;
    }

    const int ines_format = cart->buffer[0] == 'N' &&
                            cart->buffer[1] == 'E' &&
                            cart->buffer[2] == 'S' &&
                            cart->buffer[3] == 26; // MS-DOS EOF
    if (ines_format) {
        const int ines20_format = (cart->buffer[7] & 0x0C) == 0x08;
        if (ines20_format) {
            success = _parse_ines20(cart);
        } else {
            success = _parse_ines(cart);
        }

        if (! success)
            goto bail;
    } else {
        TraceLog(LOG_ERROR, "failed to load card (unrecognised ROM format)");
        success = 0;
        goto bail;
    }

    TraceLog(LOG_INFO, "done!");

bail:
    fclose(f);
    return success;
}

void cart_unload(Cart* cart) {
    free(cart->buffer);
    memset(cart, 0, sizeof(*cart));
}

static inline int _parse_ines(Cart* cart) {
    TraceLog(LOG_INFO, "ROM is iNES format");
    cart->type = kROMTYPE_INES;
    int success = 1;

    cart->prg_rom_size = cart->buffer[4]*16*1000;
    cart->chr_rom_size = cart->buffer[5]*8*1000;
    cart->prg_ram_size = cart->buffer[8]*8*1000;
    cart->nametable_arrangement = cart->buffer[6] & (1 << 0) ? kNAMETABLE_HORIZONTAL : kNAMETABLE_VERTICAL;
    cart->mapper = (cart->buffer[6] & 0xF0) >> 4 | (cart->buffer[7] & 0xF0);
    success |= _get_ines_start_addr(cart->mapper, &cart->start_addr);

    TraceLog(LOG_INFO, "ROM info:");
    TraceLog(LOG_INFO, "PRG ROM size (bytes): %zu", cart->prg_rom_size);
    TraceLog(LOG_INFO, "CHR ROM size (bytes): %zu", cart->chr_rom_size);
    TraceLog(LOG_INFO, "PRG RAM size (bytes): %zu", cart->prg_ram_size);
    TraceLog(LOG_INFO, "nametable arrangement: %s", cart->nametable_arrangement == kNAMETABLE_VERTICAL ? "vertical" : "horizontal");
    TraceLog(LOG_INFO, "mapper: %u", cart->mapper);
    TraceLog(LOG_INFO, "start addr: 0x%X", cart->start_addr);

    return success;
}

static inline int _get_ines_start_addr(uint16_t mapper, uint16_t* start_addr_out) {
    switch (mapper) {
        case 0:
            *start_addr_out = 0x8000;
            return 1;
        default:
            TraceLog(LOG_ERROR, "failed to get start address (unhandled mapper type '%u')", mapper);
            return 0;
    }
}

static inline int _parse_ines20(Cart* cart) {
    TraceLog(LOG_INFO, "ROM is iNES 2.0 format");
    cart->type = kROMTYPE_INES20;

    TraceLog(LOG_ERROR, "iNES 2.0 parsing not implemented!");
    return 0;
}

