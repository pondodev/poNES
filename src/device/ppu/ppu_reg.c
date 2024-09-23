#include "ppu_reg.h"

#include "device/memory_map.h"
#include "helpers.h"

#include "log.h"

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
    kPPUCTRL_NAMETABLE_BASE_ADDR_LSB    = 0,
    kPPUCTRL_NAMETABLE_BASE_ADDR_MSB    = 1,
    kPPUCTRL_VRAM_ADDR_INCR             = 2,
    kPPUCTRL_SPRITE_PATTERN_TABLE_ADDR  = 3,
    kPPUCTRL_BG_PATTERN_TABLE_ADDR      = 4,
    kPPUCTRL_SPRITE_SIZE                = 5,
    kPPUCTRL_RX_TX_SELECT               = 6,
    kPPUCTRL_VBLANK_NMI                 = 7,
} PPUCTRLFlag;

typedef enum {
    kPPUMASK_GRAYSCALE              = 0,
    kPPUMASK_SHOW_LEFT_BACKGROUND   = 1,
    kPPUMASK_SHOW_LEFT_SPRITES      = 2,
    kPPUMASK_SHOW_BACKGROUND        = 3,
    kPPUMASK_SHOW_SPRITES           = 4,
    // NOTE: on PAL/dendy, the bits for red/green are swapped. for now, we're
    // just emulating for NTSC (and hoping nothing PAL shits the bed)
    kPPUMASK_EMPHASIZE_RED          = 5,
    kPPUMASK_EMPHASIZE_GREEN        = 6,
    kPPUMASK_EMPHASIZE_BLUE         = 7,
} PPUMASKFlag;

typedef enum {
    // NOTE: bits 0-4 report an open bus state for the PPU
    kPPUSTATUS_SPRITE_OVERFLOW    = 5,
    kPPUSTATUS_SPRITE_0_HIT       = 6,
    kPPUSTATUS_VBLANK             = 7,
} PPUSTATUSFlag;

static struct {
    uint8_t     ppu_ctrl;
    uint8_t     ppu_mask;
    uint8_t     ppu_status;
    uint8_t     oam_addr;
    uint8_t     oam_data;
    uint8_t     ppu_scroll_x;
    uint8_t     ppu_scroll_y;
    uint16_t    ppu_addr;
} s_regs;

static struct {
    unsigned vram_addr      : 15;
    unsigned temp_vram_addr : 15;
    unsigned fine_x_scroll  : 3;
    unsigned write_latch    : 1;
} s_internal_regs;

static inline uint16_t _transform_addr(uint16_t addr) {
    addr -= PPU_REG_START;
    addr %= PPU_REG_SIZE;
    return PPU_REG_START + addr;
}

void ppu_reg_init(void) {
    s_regs.ppu_ctrl     = 0;
    s_regs.ppu_mask     = 0;
    s_regs.ppu_status   = BIT(kPPUSTATUS_SPRITE_OVERFLOW) | BIT(kPPUSTATUS_VBLANK);
    s_regs.oam_addr     = 0;
    s_regs.ppu_scroll_x = 0;
    s_regs.ppu_scroll_y = 0;
    s_regs.ppu_addr     = 0;

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
            *out = s_regs.ppu_status;
            ppu_set_vblank(0);
            s_internal_regs.write_latch = 0;
            break;
        case REG_OAMADDR:
            log_warn("attempted to read OAMADDR, which is write only");
            return 0;
        case REG_OAMDATA:
            *out = s_regs.oam_data;
            break;
        case REG_PPUSCROLL:
            log_warn("attempted to read PPUSCROLL, which is write only");
            return 0;
        case REG_PPUADDR:
            log_warn("attempted to read PPUADDR, which is write only");
            return 0;
        case REG_PPUDATA:
            *out = ppu_read_data();
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
            s_regs.ppu_ctrl = *in;
            break;
        case REG_PPUMASK:
            s_regs.ppu_mask = *in;
            break;
        case REG_PPUSTATUS:
            log_warn("attempted to write PPUSTATUS, which is read only");
            return 0;
        case REG_OAMADDR:
            s_regs.oam_addr = *in;
            break;
        case REG_OAMDATA:
            s_regs.oam_data = *in;
            ++s_regs.oam_addr;
            break;
        case REG_PPUSCROLL:
            if (s_internal_regs.write_latch)
                s_regs.ppu_scroll_y = *in;
            else
                s_regs.ppu_scroll_x = *in;

            s_internal_regs.write_latch = ! s_internal_regs.write_latch;
            break;
        case REG_PPUADDR:
        {
            uint16_t data = *in;
            uint16_t mask = 0x00FF;
            // MSB on first write, LSB on second
            if (! s_internal_regs.write_latch) {
                data <<= 8;
                mask <<= 8;
            }

            // clear the byte we're about to write to and then write it
            s_regs.ppu_addr &= ~mask;
            s_regs.ppu_addr |= data;

            s_internal_regs.write_latch = ! s_internal_regs.write_latch;
            break;
        }
        case REG_PPUDATA:
            ppu_write_data(*in);
            break;

        default:
            log_error("attempted to write memory to PPU at addr %0x%04X", addr);
            return 0;
    }

    return 1;
}

uint16_t ppu_get_base_nametable_addr(void) {
    const uint8_t mask = BIT(kPPUCTRL_NAMETABLE_BASE_ADDR_MSB) | BIT(kPPUCTRL_NAMETABLE_BASE_ADDR_LSB);
    switch (s_regs.ppu_ctrl & mask) {
        case 0x00: return 0x2000;
        case 0x01: return 0x2400;
        case 0x02: return 0x2800;
        case 0x03: return 0x2C00;
    }

    // if we hit this code path, be very afraid
    return 0x2000;
}

uint16_t ppu_get_vram_addr_increment(void) {
    if (read_bit(s_regs.ppu_ctrl, kPPUCTRL_VRAM_ADDR_INCR))
        return 1; // going across
    else
        return 32; // going down
}

uint16_t ppu_get_sprite_pattern_table_addr(void) {
    if (read_bit(s_regs.ppu_ctrl, kPPUCTRL_SPRITE_PATTERN_TABLE_ADDR))
        return 0x1000;
    else
        return 0x0000;
}

uint16_t ppu_get_bg_pattern_table_addr(void) {
    if (read_bit(s_regs.ppu_ctrl, kPPUCTRL_BG_PATTERN_TABLE_ADDR))
        return 0x1000;
    else
        return 0x0000;
}

SpriteSize ppu_get_sprite_size(void) {
    if (read_bit(s_regs.ppu_ctrl, kPPUCTRL_SPRITE_SIZE))
        return kSPRITE_SIZE_8x16;
    else
        return kSPRITE_SIZE_8x8;
}

EXTPinMode ppu_get_ext_pin_mode(void) {
    if (read_bit(s_regs.ppu_ctrl, kPPUCTRL_RX_TX_SELECT))
        return kEXT_PIN_MODE_TX;
    else
        return kEXT_PIN_MODE_RX;
}

int ppu_get_vblank_nmi_enabled(void) {
    return read_bit(s_regs.ppu_ctrl, kPPUCTRL_VBLANK_NMI);
}

int ppu_get_grayscale_mode(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_GRAYSCALE);
}

int ppu_get_show_left_background(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_SHOW_LEFT_BACKGROUND);
}

int ppu_get_show_left_sprites(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_SHOW_LEFT_SPRITES);
}

int ppu_get_show_background(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_SHOW_BACKGROUND);
}

int ppu_get_show_sprites(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_SHOW_SPRITES);
}

int ppu_get_emphasize_red(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_EMPHASIZE_RED);
}

int ppu_get_emphasize_green(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_EMPHASIZE_GREEN);
}

int ppu_get_emphasize_blue(void) {
    return read_bit(s_regs.ppu_mask, kPPUMASK_EMPHASIZE_BLUE);
}

void ppu_set_sprite_overflow(int value) {
    write_bit(&s_regs.ppu_status, kPPUSTATUS_SPRITE_OVERFLOW, value);
}

void ppu_set_sprite_0_hit(int value) {
    write_bit(&s_regs.ppu_status, kPPUSTATUS_SPRITE_0_HIT, value);
}

void ppu_set_vblank(int value) {
    write_bit(&s_regs.ppu_status, kPPUSTATUS_VBLANK, value);
}

uint8_t ppu_get_oam_addr(void) {
    return s_regs.oam_addr;
}

uint8_t ppu_get_oam_data(void) {
    return s_regs.oam_data;
}

uint8_t ppu_get_scroll_x(void) {
    return s_regs.ppu_scroll_x;
}

uint8_t ppu_get_scroll_y(void) {
    return s_regs.ppu_scroll_y;
}

uint16_t ppu_get_addr(void) {
    return s_regs.ppu_addr;
}

void ppu_write_data(uint8_t data) {
    // TODO
    (void)data;
}

uint8_t ppu_read_data(void) {
    // TODO
    return 0;
}

