#include "ppu.h"

#include "raylib.h"

#include "device.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define COLOR_PALETTE_SIZE 0x40
#define COLOR_PALETTE_NAME_MAX_LEN 30
#define DEFAULT_COLOR_PALETTE_NAME "default"
typedef struct {
    char        name[COLOR_PALETTE_NAME_MAX_LEN];
    Color       palette[COLOR_PALETTE_SIZE];
} ColorPalette;

static ColorPalette s_current_palette;
static ColorPalette s_default_palette = {
    .name = "default",
};

static uint32_t s_default_palette_hex[COLOR_PALETTE_SIZE] = {
    0x6D6D6DFF, 0x002492FF, 0x0000DBFF, 0x6D49DBFF, 0x92006DFF, 0xB6006DFF, 0xB62400FF, 0x924900FF, 0x6D4900FF, 0x244900FF, 0x006D24FF, 0x009200FF, 0x004949FF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xB6B6B6FF, 0x006DDBFF, 0x0049FFFF, 0x9200FFFF, 0xB600FFFF, 0xFF0092FF, 0xFF0000FF, 0xDB6D00FF, 0x926D00FF, 0x249200FF, 0x009200FF, 0x00B66DFF, 0x009292FF, 0x242424FF, 0x000000FF, 0x000000FF,
    0xFFFFFFFF, 0x6DB6FFFF, 0x9292FFFF, 0xDB6DFFFF, 0xFF00FFFF, 0xFF6DFFFF, 0xFF9200FF, 0xFFB600FF, 0xDBDB00FF, 0x6DDB00FF, 0x00FF00FF, 0x49FFDBFF, 0x00FFFFFF, 0x494949FF, 0x000000FF, 0x000000FF,
    0xFFFFFFFF, 0xB6DBFFFF, 0xDBB6FFFF, 0xFFB6FFFF, 0xFF92FFFF, 0xFFB6B6FF, 0xFFDB92FF, 0xFFFF49FF, 0xFFFF6DFF, 0xB6FF49FF, 0x92FF6DFF, 0x49FFDBFF, 0x92DBFFFF, 0x929292FF, 0x000000FF, 0x000000FF,
};

static void _load_default_color_palette(void);
static Color _hex_to_color(uint32_t hex);
static void _trim_newline(char* str, size_t len);

void ppu_init(void) {
    memset(s_current_palette.name, '\0', sizeof(s_current_palette.name[0])*COLOR_PALETTE_NAME_MAX_LEN);
    memset(s_current_palette.palette, '\0', sizeof(s_current_palette.palette[0])*COLOR_PALETTE_SIZE);

    for (size_t i = 0; i < COLOR_PALETTE_SIZE; ++i) {
        s_default_palette.palette[i] = _hex_to_color(s_default_palette_hex[i]);
    }
}

void ppu_load_color_palette(const char* path) {
    if (path == NULL) {
        TraceLog(LOG_INFO, "loading default colour palette");
        _load_default_color_palette();
        return;
    }

    TraceLog(LOG_INFO, "loading color palette from file '%s'...", path);

    int success = 1;

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        TraceLog(LOG_ERROR, "failed to load colour palette (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    char* name = NULL;
    size_t name_len = 0;
    if (! getline(&name, &name_len, f)) {
        if (feof(f)) {
            TraceLog(LOG_ERROR, "failed to load colour palette (unexpected EOF)");
        } else {
            TraceLog(LOG_ERROR, "failed to load colour palette (%s)", strerror(ferror(f)));
        }

        success = 0;
        goto bail;
    }

    strncpy(s_current_palette.name, name, COLOR_PALETTE_NAME_MAX_LEN-1);
    _trim_newline(s_current_palette.name, COLOR_PALETTE_NAME_MAX_LEN);
    TraceLog(LOG_INFO, "found colour palette '%s', parsing...", s_current_palette.name);

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
        s_current_palette.palette[parsed_colors++] = _hex_to_color(hex_num);
    }

    const int ferr = ferror(f);
    if (ferr != 0) {
        TraceLog(LOG_ERROR, "failed to load colour palette (%s)", strerror(ferr));
        success = 0;
        goto bail;
    }

    TraceLog(LOG_INFO, "done!");

bail:
    fclose(f);

    if (! success) {
        TraceLog(LOG_WARNING, "failed to load colour palette, falling back to default");
        _load_default_color_palette();
    }
}

void ppu_draw(void) {
    for (size_t i = 0; i < VIDEO_BUFFER_SIZE_BYTES; ++i) {
        // TODO
    }
}

void ppu_draw_current_palette(void) {
    const size_t rows = 4;
    const size_t cols = COLOR_PALETTE_SIZE/rows;
    const Rectangle origin_rect = {
        .x = 0,
        .y = 0,
        .width = 30,
        .height = 30,
    };

    for (size_t i = 0; i < COLOR_PALETTE_SIZE; ++i) {
        const int x = i%cols;
        const int y = i/cols;
        const Rectangle color_rect = {
            .x = origin_rect.x + (x*origin_rect.width),
            .y = origin_rect.y + (y*origin_rect.height),
            .width = origin_rect.width,
            .height = origin_rect.height,
        };

        DrawRectangleRec(color_rect, s_current_palette.palette[i]);
    }
}

static void _load_default_color_palette(void) {
    memcpy(s_current_palette.name, s_default_palette.name, sizeof(s_current_palette.name[0])*COLOR_PALETTE_NAME_MAX_LEN);
    memcpy(s_current_palette.palette, s_default_palette.palette, sizeof(s_current_palette.palette[0])*COLOR_PALETTE_SIZE);
}

static Color _hex_to_color(uint32_t hex) {
    Color temp;
    memcpy(&temp, &hex, sizeof(hex));

    // copying the value in will give us the colour components
    // backwards, so flip it here
    const Color c = {
        .r = temp.a,
        .g = temp.b,
        .b = temp.g,
        .a = temp.r,
    };
    return c;
}

static void _trim_newline(char* str, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n')
            str[i] = '\0';
    }
}

