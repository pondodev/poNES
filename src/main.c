#include "device/device.h"
#include "device/cart/cart.h"
#include "device/ppu/ppu.h"
#include "device/ppu/color_palette.h"
#include "platform/platform.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

#define MIN_EXPECTED_ARG_COUNT 2
#define MAX_EXPECTED_ARG_COUNT 3

int main(int argc, char* argv[]) {
    log_set_level(LOG_TRACE);

    if (argc < MIN_EXPECTED_ARG_COUNT || argc > MAX_EXPECTED_ARG_COUNT) {
        log_error("incorrect arg count. expected between %d and %d (got %d)\n", MIN_EXPECTED_ARG_COUNT, MAX_EXPECTED_ARG_COUNT, argc);
        return 1;
    }

    Cart cart;
    if (! cart_load(argv[1], &cart))
        return 1;

    platform_init();
    device_init();
    device_load_cart(&cart);

    const char* palette_path = argc == 3 ? argv[2] : NULL;
    color_palette_from_file(palette_path);

    uint32_t buffer[VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT];
    size_t pixel = 0;
    while (platform_is_running()) {
        platform_poll_events();

        memset(buffer, 0, sizeof(buffer[0])*VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT);
        pixel = (pixel+1) % (VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT);
        buffer[pixel] = 0xFFFFFFFF;
        platform_update_frame_buffer(buffer);

        device_exec();
        platform_draw();
    }

    cart_unload(&cart);
    platform_cleanup();

    return 0;
}
