#ifndef COLOR_PALETTE_H
#define COLOR_PALETTE_H

#include <stdint.h>

#define COLOR_PALETTE_SIZE 0x40
#define COLOR_PALETTE_NAME_MAX_LEN 30

int color_palette_from_file(const char* path);
uint32_t color_palette_get_color(uint8_t index);

#endif

