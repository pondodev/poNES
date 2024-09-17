#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include <stdlib.h>

void ram_init(void);
int ram_read8(uint16_t addr, uint8_t* out);
int ram_write8(uint16_t addr, const uint8_t* in);

#endif

