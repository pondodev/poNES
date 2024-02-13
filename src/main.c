#include "device.h"
#include "cart.h"

#include "raylib.h"

#include <stdio.h>

#define WINDOW_SIZE_MULTIPLIER 2
#define WINDOW_WIDTH VIDEO_BUFFER_WIDTH*WINDOW_SIZE_MULTIPLIER
#define WINDOW_HEIGHT VIDEO_BUFFER_HEIGHT*WINDOW_SIZE_MULTIPLIER
#define WINDOW_TITLE "poNES"

#define EXPECTED_ARG_COUNT 2

int main(int argc, char* argv[]) {
    SetTraceLogLevel(LOG_ALL);

    if (argc != EXPECTED_ARG_COUNT) {
        TraceLog(LOG_ERROR, "incorrect arg count. expected %d (got %d)\n", EXPECTED_ARG_COUNT, argc);
        return 1;
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    device_init();

    Cart cart;
    if (! cart_load(argv[1], &cart))
        return 1;

    while (! WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
        }
        EndDrawing();
    }

    cart_unload(&cart);
    CloseWindow();

    return 0;
}
