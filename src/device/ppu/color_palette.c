#include "color_palette.h"

#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char        name[COLOR_PALETTE_NAME_MAX_LEN];
    uint32_t    palette[COLOR_PALETTE_SIZE];
} ColorPalette;

#define DEFAULT_COLOR_PALETTE_NAME "default"
#define DEFAULT_COLOR_PALETTE \
    (ColorPalette) { \
        .name = DEFAULT_COLOR_PALETTE_NAME, \
        .palette = { \
            0x6D6D6DFF, 0x002492FF, 0x0000DBFF, 0x6D49DBFF, 0x92006DFF, 0xB6006DFF, 0xB62400FF, 0x924900FF, 0x6D4900FF, 0x244900FF, 0x006D24FF, 0x009200FF, 0x004949FF, 0x000000FF, 0x000000FF, 0x000000FF, \
            0xB6B6B6FF, 0x006DDBFF, 0x0049FFFF, 0x9200FFFF, 0xB600FFFF, 0xFF0092FF, 0xFF0000FF, 0xDB6D00FF, 0x926D00FF, 0x249200FF, 0x009200FF, 0x00B66DFF, 0x009292FF, 0x242424FF, 0x000000FF, 0x000000FF, \
            0xFFFFFFFF, 0x6DB6FFFF, 0x9292FFFF, 0xDB6DFFFF, 0xFF00FFFF, 0xFF6DFFFF, 0xFF9200FF, 0xFFB600FF, 0xDBDB00FF, 0x6DDB00FF, 0x00FF00FF, 0x49FFDBFF, 0x00FFFFFF, 0x494949FF, 0x000000FF, 0x000000FF, \
            0xFFFFFFFF, 0xB6DBFFFF, 0xDBB6FFFF, 0xFFB6FFFF, 0xFF92FFFF, 0xFFB6B6FF, 0xFFDB92FF, 0xFFFF49FF, 0xFFFF6DFF, 0xB6FF49FF, 0x92FF6DFF, 0x49FFDBFF, 0x92DBFFFF, 0x929292FF, 0x000000FF, 0x000000FF, \
        }, \
    }

static ColorPalette s_current_palette = DEFAULT_COLOR_PALETTE;

static inline void _trim_newline(char* str, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n')
            str[i] = '\0';
    }
}

int color_palette_from_file(const char* path) {
    if (path == NULL) {
        log_warn("custom color palette file not provided");
        return 0;
    }

    log_info("loading color palette from file '%s'...", path);

    int success = 1;

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        log_error("failed to load colour palette (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    char* name = NULL;
    size_t name_len = 0;
    if (! getline(&name, &name_len, f)) {
        if (feof(f))
            log_error("failed to load colour palette (unexpected EOF)");
        else
            log_error("failed to load colour palette (%s)", strerror(ferror(f)));

        success = 0;
        goto bail;
    }

    strncpy(s_current_palette.name, name, COLOR_PALETTE_NAME_MAX_LEN-1);
    _trim_newline(s_current_palette.name, COLOR_PALETTE_NAME_MAX_LEN);
    log_info("found colour palette '%s', parsing...", s_current_palette.name);

    int c = fgetc(f);
    size_t parsed_colors = 0;
    while (c != EOF && parsed_colors < COLOR_PALETTE_SIZE) {
        if (c != '#') {
            c = fgetc(f);
            continue;
        }

        char hex_str[9] = {'\0'};
        for (size_t i = 0; i < 8; ++i) {
            c = fgetc(f);
            if (c == EOF)
                break;

            hex_str[i] = c;
        }

        // invalid value read from file
        if (hex_str[7] == '\0')
            continue;

        const uint32_t hex_num = strtol(hex_str, NULL, 16);
        s_current_palette.palette[parsed_colors++] = hex_num;
    }

    const int ferr = ferror(f);
    if (ferr != 0) {
        log_error("failed to load colour palette (%s)", strerror(ferr));
        success = 0;
        goto bail;
    }

    log_info("done!");

bail:
    fclose(f);

    if (! success)
        log_warn("failed to load colour palette");

    return success;
}

uint32_t color_palette_get_color(uint8_t index) {
    if (index >= COLOR_PALETTE_SIZE)
        return 0;

    return s_current_palette.palette[index];
}

