#include "cart.h"

#include "raylib.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

int cart_load(const char* path, Cart* cart) {
    TraceLog(LOG_INFO, "loading cart from path '%s'...", path);

    int success = 1;

    if (cart == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (cart cannot be NULL)");
        return 0;
    }

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        TraceLog(LOG_ERROR, "failed to load cart (%s)", strerror(errno));
        success = 0;
        goto bail;
    }

    fseek(f, 0, SEEK_END);
    cart->size = ftell(f);
    rewind(f);

    TraceLog(LOG_INFO, "reading %zu bytes...", cart->size);

    cart->buffer = malloc(cart->size);
    const size_t read_bytes = fread(cart->buffer, sizeof(cart->buffer[0]), cart->size, f);
    if (cart->size != read_bytes) {
        TraceLog(LOG_ERROR, "failed to load cart (expected %zu, got %zu)", cart->size, read_bytes);
        success = 0;
        goto bail;
    }

    TraceLog(LOG_INFO, "done!");

bail:
    fclose(f);
    return success;
}

void cart_unload(Cart* cart) {
    cart->size = 0;
    free(cart->buffer);
    cart->buffer = NULL;
}

