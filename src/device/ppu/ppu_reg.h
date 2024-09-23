#ifndef PPU_REG_H
#define PPU_REG_H

#include <stdint.h>

typedef enum {
    kSPRITE_SIZE_8x8,
    kSPRITE_SIZE_8x16,
} SpriteSize;

typedef enum {
    kEXT_PIN_MODE_TX,
    kEXT_PIN_MODE_RX,
} EXTPinMode;

void ppu_reg_init(void);
int ppu_reg_read8(uint16_t addr, uint8_t* out);
int ppu_reg_write8(uint16_t addr, const uint8_t* in);

// PPUCTRL
uint16_t ppu_get_base_nametable_addr(void);
uint16_t ppu_get_vram_addr_increment(void);
uint16_t ppu_get_sprite_pattern_table_addr(void);
uint16_t ppu_get_bg_pattern_table_addr(void);
SpriteSize ppu_get_sprite_size(void);
EXTPinMode ppu_get_ext_pin_mode(void);
int ppu_get_vblank_nmi_enabled(void);

// PPUMASK
int ppu_get_grayscale_mode(void);
int ppu_get_show_left_background(void);
int ppu_get_show_left_sprites(void);
int ppu_get_show_background(void);
int ppu_get_show_sprites(void);
int ppu_get_emphasize_red(void);
int ppu_get_emphasize_green(void);
int ppu_get_emphasize_blue(void);

// PPUSTATUS
void ppu_set_sprite_overflow(int value);
void ppu_set_sprite_0_hit(int value);
void ppu_set_vblank(int value);

// OAMADDR
uint8_t ppu_get_oam_addr(void);

// OAMDATA
uint8_t ppu_get_oam_data(void);

// PPUSCROLL
uint8_t ppu_get_scroll_x(void);
uint8_t ppu_get_scroll_y(void);

// PPUADDR
uint16_t ppu_get_addr(void);

// PPUDATA
void ppu_write_data(uint8_t data);
uint8_t ppu_read_data(void);

#endif

