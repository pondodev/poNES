#ifndef PPU_H
#define PPU_H

void ppu_init(void);
void ppu_load_color_palette(const char* path);
void ppu_draw(void);
void ppu_draw_current_palette(void);

#endif

