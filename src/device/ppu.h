#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdlib.h>

#define VIDEO_BUFFER_WIDTH  256
#define VIDEO_BUFFER_HEIGHT 240

void ppu_init(void);

int ppu_reg_read8(uint16_t addr, uint8_t* out);
int ppu_reg_write8(uint16_t addr, const uint8_t* in);

const uint32_t* ppu_get_buffer(void);

#endif

