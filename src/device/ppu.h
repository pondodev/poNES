#ifndef PPU_H
#define PPU_H

#include <stdint.h>

#define VIDEO_BUFFER_WIDTH  256
#define VIDEO_BUFFER_HEIGHT 240

void ppu_init(void);
void ppu_load_color_palette(const char* path);
uint32_t ppu_get_color(uint8_t index);
const uint32_t* ppu_get_buffer(void);

#endif

