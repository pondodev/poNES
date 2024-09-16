#include "cart.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "ines.h"
#include "log.h"

static inline int _parse_ines(Cart* cart);
static inline int _parse_ines20(Cart* cart);

int cart_load(const char* path, Cart* cart) {
    log_info("loading cart from path '%s'...", path);

    int success = 1;

    if (cart == NULL) {
        log_error("failed to load cart (cart cannot be NULL)");
        return 0;
    }

    memset(cart, 0, sizeof(*cart));

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        log_error("failed to load cart (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    fseek(f, 0, SEEK_END);
    cart->buffer_size = ftell(f);
    rewind(f);

    log_info("reading %zu bytes...", cart->buffer_size);

    cart->buffer = malloc(cart->buffer_size);
    const size_t read_bytes = fread(cart->buffer, sizeof(cart->buffer[0]), cart->buffer_size, f);
    if (cart->buffer_size != read_bytes) {
        log_error("failed to load cart (expected %zu, got %zu)", cart->buffer_size, read_bytes);
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
        log_error("failed to load cart (unrecognised ROM format)");
        success = 0;
        goto bail;
    }

    log_info("done!");

bail:
    fclose(f);
    return success;
}

void cart_unload(Cart* cart) {
    free(cart->buffer);
    memset(cart, 0, sizeof(*cart));
}

int cart_read(uint16_t addr, void* out, size_t n) {
    (void)addr;
    (void)out;
    (void)n;

    // TODO
    return 0;
}

int cart_write(uint16_t addr, const void* in, size_t n) {
    (void)addr;
    (void)in;
    (void)n;

    // TODO
    return 0;
}

uint16_t cart_entrypoint(Cart* cart) {
    return mapper_get_start_addr(cart->mapper);
}

static inline int _parse_ines(Cart* cart) {
    log_info("ROM is iNES format");
    cart->format = kROMFORMAT_INES;
    int success = 1;

    cart->format_header = ines_load(cart->buffer, cart->buffer_size);
    if (cart->format_header == NULL) {
        log_error("failed to parse header");
        success = 0;
        goto bail;
    }

    const uint8_t mapper_num = ines_mapper(cart->format_header);
    cart->mapper = mapper_get_type(mapper_num);
    if (cart->mapper == kCARTMAPPER_UNKNOWN) {
        log_error("unknown mapper type '%u'", mapper_num);
        success = 0;
        goto bail;
    }

    cart->prg_rom_start = ines_prg_rom_start(cart->format_header);
    cart->prg_rom_size  = ines_prg_rom_size_bytes(cart->format_header);
    cart->chr_rom_start = ines_chr_rom_start(cart->format_header);
    cart->chr_rom_size  = ines_chr_rom_size_bytes(cart->format_header);
    cart->prg_ram_size  = 0; // TODO: implement

    log_info("ROM info:");
    log_info("PRG ROM start: 0x%04X", cart->prg_rom_start);
    log_info("PRG ROM size (bytes): %zu", cart->prg_rom_size);
    log_info("CHR ROM start: 0x%04X", cart->chr_rom_start);
    log_info("CHR ROM size (bytes): %zu", cart->chr_rom_size);
    log_info("PRG RAM size (bytes): %zu", cart->prg_ram_size);

bail:
    return success;
}

static inline int _parse_ines20(Cart* cart) {
    log_info("ROM is iNES 2.0 format");
    cart->format = kROMFORMAT_INES20;

    log_error("iNES 2.0 parsing not implemented!");
    return 0;
}

