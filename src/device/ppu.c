#include "ppu.h"

#include "color_palette.h"
#include "memory_map.h"
#include "helpers.h"

#include "log.h"

#include <string.h>

#define VIDEO_BUFFER_SIZE VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT

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

static uint32_t s_video_buffer[VIDEO_BUFFER_SIZE];

static inline uint16_t _transform_addr(uint16_t addr);

void ppu_init(void) {
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

const uint32_t* ppu_get_buffer(void) {
    return s_video_buffer;
}

static inline uint16_t _transform_addr(uint16_t addr) {
    addr -= PPU_REG_START;
    addr %= PPU_REG_SIZE;
    return PPU_REG_START + addr;
}

