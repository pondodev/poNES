#include "device/device.h"
#include "device/cart/cart.h"
#include "device/ppu.h"
#include "platform/platform.h"

#include "raylib.h"

#include <stdio.h>

#define MIN_EXPECTED_ARG_COUNT 2
#define MAX_EXPECTED_ARG_COUNT 3

int main(int argc, char* argv[]) {
    SetTraceLogLevel(LOG_ALL);

    if (argc < MIN_EXPECTED_ARG_COUNT || argc > MAX_EXPECTED_ARG_COUNT) {
        TraceLog(LOG_ERROR, "incorrect arg count. expected between %d and %d (got %d)\n", MIN_EXPECTED_ARG_COUNT, MAX_EXPECTED_ARG_COUNT, argc);
        return 1;
    }

    Cart cart;
    if (! cart_load(argv[1], &cart))
        return 1;

    platform_init();
    device_init();
    device_load_cart(&cart);

    const char* palette_path = argc == 3 ? argv[2] : NULL;
    ppu_load_color_palette(palette_path);

    while (platform_is_running()) {
        device_exec();
        platform_draw();
    }

    cart_unload(&cart);
    CloseWindow();

    return 0;
}
