#include "platform.h"

#include "raylib.h"

#include "device/ppu.h"

#define WINDOW_SIZE_MULTIPLIER 2
#define WINDOW_WIDTH VIDEO_BUFFER_WIDTH*WINDOW_SIZE_MULTIPLIER
#define WINDOW_HEIGHT VIDEO_BUFFER_HEIGHT*WINDOW_SIZE_MULTIPLIER
#define WINDOW_TITLE "poNES"

void platform_init(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
}

void platform_cleanup(void) {
    CloseWindow();
}

int platform_is_running(void) {
    return ! WindowShouldClose();
}

void platform_draw(void) {
    BeginDrawing();
    {
        ClearBackground(BLACK);
        // TODO: drawing
    }
    EndDrawing();
}

