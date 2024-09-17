#include "ppu.h"

#include "memory_map.h"
#include "helpers.h"

#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define VIDEO_BUFFER_SIZE VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT
#define COLOR_PALETTE_SIZE 0x40
#define COLOR_PALETTE_NAME_MAX_LEN 30
#define DEFAULT_COLOR_PALETTE_NAME "default"

#define REG_PPUCTRL     0x2000
#define REG_PPUMASK     0x2001
#define REG_PPUSTATUS   0x2002
#define REG_OAMADDR     0x2003
#define REG_OAMDATA     0x2004
#define REG_PPUSCROLL   0x2005
#define REG_PPUADDR     0x2006
#define REG_PPUDATA     0x2007
#define REG_OAMDMA      0x4014 // TODO: should this be here?

typedef enum {
    kPPU_STATUS_FLAG_SPRITE_OVERFLOW    = 5,
    kPPU_STATUS_FLAG_SPRITE_0_HIT       = 6,
    kPPU_STATUS_FLAG_VBLANK             = 7,
} PPUStatusFlag;

typedef struct __attribute__((__packed__)) {
    uint8_t pos_y;
    uint8_t tile_idx;
    uint8_t attr;
    uint8_t pos_x;
} OAMSprite;

typedef struct {
    char        name[COLOR_PALETTE_NAME_MAX_LEN];
    uint32_t    palette[COLOR_PALETTE_SIZE];
} ColorPalette;

static struct {
    uint8_t ppu_ctrl;
    uint8_t ppu_mask;
    uint8_t ppu_status;
    uint8_t oam_addr;
    uint8_t oam_data;
    uint8_t ppu_scroll;
    uint8_t ppu_addr;
    uint8_t ppu_data;
} s_regs;

static struct {
    unsigned vram_addr      : 15;
    unsigned temp_vram_addr : 15;
    unsigned fine_x_scroll  : 3;
    unsigned write_latch    : 1;
} s_internal_regs;

#define OAM_SPRITE_COUNT 64
static OAMSprite s_oam[OAM_SPRITE_COUNT];

static ColorPalette s_current_palette;
static ColorPalette s_default_palette = {
    .name = DEFAULT_COLOR_PALETTE_NAME,
    .palette = {
        0x6D6D6DFF, 0x002492FF, 0x0000DBFF, 0x6D49DBFF, 0x92006DFF, 0xB6006DFF, 0xB62400FF, 0x924900FF, 0x6D4900FF, 0x244900FF, 0x006D24FF, 0x009200FF, 0x004949FF, 0x000000FF, 0x000000FF, 0x000000FF,
        0xB6B6B6FF, 0x006DDBFF, 0x0049FFFF, 0x9200FFFF, 0xB600FFFF, 0xFF0092FF, 0xFF0000FF, 0xDB6D00FF, 0x926D00FF, 0x249200FF, 0x009200FF, 0x00B66DFF, 0x009292FF, 0x242424FF, 0x000000FF, 0x000000FF,
        0xFFFFFFFF, 0x6DB6FFFF, 0x9292FFFF, 0xDB6DFFFF, 0xFF00FFFF, 0xFF6DFFFF, 0xFF9200FF, 0xFFB600FF, 0xDBDB00FF, 0x6DDB00FF, 0x00FF00FF, 0x49FFDBFF, 0x00FFFFFF, 0x494949FF, 0x000000FF, 0x000000FF,
        0xFFFFFFFF, 0xB6DBFFFF, 0xDBB6FFFF, 0xFFB6FFFF, 0xFF92FFFF, 0xFFB6B6FF, 0xFFDB92FF, 0xFFFF49FF, 0xFFFF6DFF, 0xB6FF49FF, 0x92FF6DFF, 0x49FFDBFF, 0x92DBFFFF, 0x929292FF, 0x000000FF, 0x000000FF,
    },
};

static uint32_t s_video_buffer[VIDEO_BUFFER_SIZE];

static void _load_default_color_palette(void);
static void _trim_newline(char* str, size_t len);

static inline uint16_t _transform_addr(uint16_t addr);

void ppu_init(void) {
    memset(s_current_palette.name, '\0', sizeof(s_current_palette.name[0])*COLOR_PALETTE_NAME_MAX_LEN);
    memset(s_current_palette.palette, 0, sizeof(s_current_palette.palette[0])*COLOR_PALETTE_SIZE);
    memset(s_video_buffer, 0, sizeof(s_video_buffer[0])*VIDEO_BUFFER_SIZE);

    s_regs.ppu_ctrl     = 0;
    s_regs.ppu_mask     = 0;
    s_regs.ppu_status   = BIT(kPPU_STATUS_FLAG_SPRITE_OVERFLOW) | BIT(kPPU_STATUS_FLAG_VBLANK);
    s_regs.oam_addr     = 0;
    s_regs.ppu_scroll   = 0;
    s_regs.ppu_addr     = 0;
    s_regs.ppu_data     = 0;

    s_internal_regs.write_latch = 0;
}

int ppu_reg_read8(uint16_t addr, uint8_t* out) {
    (void)out;
    switch (_transform_addr(addr)) {
        case REG_PPUCTRL:
            log_warn("attempted to read PPUCTRL, which is write only");
            return 0;
        case REG_PPUMASK:
            log_warn("attempted to read PPUMASK, which is write only");
            return 0;
        case REG_PPUSTATUS:
            // TODO
            break;
        case REG_OAMADDR:
            log_warn("attempted to read OAMADDR, which is write only");
            return 0;
        case REG_OAMDATA:
            // TODO
            break;
        case REG_PPUSCROLL:
            log_warn("attempted to read PPUSCROLL, which is write only");
            return 0;
        case REG_PPUADDR:
            log_warn("attempted to read PPUADDR, which is write only");
            return 0;
        case REG_PPUDATA:
            // TODO
            break;

        default:
            log_error("attempted to read memory from PPU at addr %0x%04X", addr);
            return 0;
    }

    return 1;
}

int ppu_reg_write8(uint16_t addr, const uint8_t* in) {
    (void)in;

    switch (_transform_addr(addr)) {
        case REG_PPUCTRL:
            // TODO
            break;
        case REG_PPUMASK:
            // TODO
            break;
        case REG_PPUSTATUS:
            log_warn("attempted to write PPUSTATUS, which is read only");
            return 0;
        case REG_OAMADDR:
            // TODO
            break;
        case REG_OAMDATA:
            // TODO
            break;
        case REG_PPUSCROLL:
            // TODO
            break;
        case REG_PPUADDR:
            // TODO
            break;
        case REG_PPUDATA:
            // TODO
            break;

        default:
            log_error("attempted to write memory to PPU at addr %0x%04X", addr);
            return 0;
    }

    return 1;
}

void ppu_load_color_palette(const char* path) {
    if (path == NULL) {
        log_info("loading default colour palette");
        _load_default_color_palette();
        return;
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

    if (! success) {
        log_warn("failed to load colour palette, falling back to default");
        _load_default_color_palette();
    }
}

uint32_t ppu_get_color(uint8_t index) {
    if (index >= COLOR_PALETTE_SIZE)
        return 0;

    return s_current_palette.palette[index];
}

const uint32_t* ppu_get_buffer(void) {
    return s_video_buffer;
}

static void _load_default_color_palette(void) {
    memcpy(s_current_palette.name, s_default_palette.name, sizeof(s_current_palette.name[0])*COLOR_PALETTE_NAME_MAX_LEN);
    memcpy(s_current_palette.palette, s_default_palette.palette, sizeof(s_current_palette.palette[0])*COLOR_PALETTE_SIZE);
}

static void _trim_newline(char* str, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n')
            str[i] = '\0';
    }
}

static inline uint16_t _transform_addr(uint16_t addr) {
    addr -= PPU_REG_START;
    addr %= PPU_REG_SIZE;
    return PPU_REG_START + addr;
}

