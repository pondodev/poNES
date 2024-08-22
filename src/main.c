#include "device.h"
#include "cart.h"
#include "ppu.h"

#include "raylib.h"

#include <stdio.h>

#define WINDOW_SIZE_MULTIPLIER 2
#define WINDOW_WIDTH VIDEO_BUFFER_WIDTH*WINDOW_SIZE_MULTIPLIER
#define WINDOW_HEIGHT VIDEO_BUFFER_HEIGHT*WINDOW_SIZE_MULTIPLIER
#define WINDOW_TITLE "poNES"

#define MIN_EXPECTED_ARG_COUNT 2
#define MAX_EXPECTED_ARG_COUNT 3

int main(int argc, char* argv[]) {
    SetTraceLogLevel(LOG_ALL);

    if (argc < MIN_EXPECTED_ARG_COUNT || argc > MAX_EXPECTED_ARG_COUNT) {
        TraceLog(LOG_ERROR, "incorrect arg count. expected between %d and %d (got %d)\n", MIN_EXPECTED_ARG_COUNT, MAX_EXPECTED_ARG_COUNT, argc);
        return 1;
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Cart cart;
    if (! cart_load(argv[1], &cart))
        return 1;

    device_init();
    device_load_cart(&cart);
    ppu_init();

    const char* palette_path = argc == 3 ? argv[2] : NULL;
    ppu_load_color_palette(palette_path);

    while (! WindowShouldClose()) {
        device_exec();

        BeginDrawing();
        {
            ClearBackground(BLACK);
            ppu_draw();
            ppu_draw_current_palette();
        }
        EndDrawing();
    }

    cart_unload(&cart);
    CloseWindow();

    return 0;
}
