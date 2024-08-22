#include "cart.h"

#include "raylib.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "ines.h"

static inline int _parse_ines(Cart* cart);
static inline int _parse_ines20(Cart* cart);

int cart_load(const char* path, Cart* cart) {
    TraceLog(LOG_INFO, "loading cart from path '%s'...", path);

    int success = 1;

    if (cart == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (cart cannot be NULL)");
        return 0;
    }

    memset(cart, 0, sizeof(*cart));

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    fseek(f, 0, SEEK_END);
    cart->buffer_size = ftell(f);
    rewind(f);

    TraceLog(LOG_INFO, "reading %zu bytes...", cart->buffer_size);

    cart->buffer = malloc(cart->buffer_size);
    const size_t read_bytes = fread(cart->buffer, sizeof(cart->buffer[0]), cart->buffer_size, f);
    if (cart->buffer_size != read_bytes) {
        TraceLog(LOG_ERROR, "failed to load cart (expected %zu, got %zu)", cart->buffer_size, read_bytes);
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

uint16_t cart_entrypoint(Cart* cart) {
    return mapper_get_start_addr(cart->mapper);
}

static inline int _parse_ines(Cart* cart) {
    TraceLog(LOG_INFO, "ROM is iNES format");
    cart->format = kROMFORMAT_INES;
    int success = 1;

    cart->format_header = ines_load(cart->buffer, cart->buffer_size);
    if (cart->format_header == NULL) {
        TraceLog(LOG_ERROR, "failed to parse header");
        success = 0;
        goto bail;
    }

    const uint8_t mapper_num = ines_mapper(cart->format_header);
    cart->mapper = mapper_get_type(mapper_num);
    if (cart->mapper == kCARTMAPPER_UNKNOWN) {
        TraceLog(LOG_ERROR, "unknown mapper type '%u'", mapper_num);
        success = 0;
        goto bail;
    }

    cart->prg_rom_start = ines_prg_rom_start(cart->format_header);
    cart->prg_rom_size  = ines_prg_rom_size_bytes(cart->format_header);
    cart->chr_rom_start = ines_chr_rom_start(cart->format_header);
    cart->chr_rom_size  = ines_chr_rom_size_bytes(cart->format_header);
    cart->prg_ram_size  = 0; // TODO: implement

    TraceLog(LOG_INFO, "ROM info:");
    TraceLog(LOG_INFO, "PRG ROM start: 0x%04X", cart->prg_rom_start);
    TraceLog(LOG_INFO, "PRG ROM size (bytes): %zu", cart->prg_rom_size);
    TraceLog(LOG_INFO, "CHR ROM start: 0x%04X", cart->chr_rom_start);
    TraceLog(LOG_INFO, "CHR ROM size (bytes): %zu", cart->chr_rom_size);
    TraceLog(LOG_INFO, "PRG RAM size (bytes): %zu", cart->prg_ram_size);

bail:
    return success;
}

static inline int _parse_ines20(Cart* cart) {
    TraceLog(LOG_INFO, "ROM is iNES 2.0 format");
    cart->format = kROMFORMAT_INES20;

    TraceLog(LOG_ERROR, "iNES 2.0 parsing not implemented!");
    return 0;
}

