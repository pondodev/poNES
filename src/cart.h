#ifndef CART_H
#define CART_H

#include <stdlib.h>

typedef struct {
    size_t      size;
    uint8_t*    buffer;
} Cart;

int cart_load(const char* path, Cart* cart);
void cart_unload(Cart* cart);

#endif

