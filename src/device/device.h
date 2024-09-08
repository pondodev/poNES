#ifndef DEVICE_H
#define DEVICE_H

#include "cart/cart.h"

#include <stdint.h>

typedef struct {
    // registers
    uint16_t    pc;
    uint8_t     sp;
    uint8_t     acc;
    uint8_t     x;
    uint8_t     y;
    uint8_t     status;

    Cart*       cart;
} Device;

extern Device g_device;

void device_init(void);
void device_load_cart(Cart* cart);
void device_exec(void);
uint8_t device_read_mem(uint16_t addr);

#endif

