#include "device.h"

#include <string.h>

Device g_device;

void device_init(void) {
    g_device = (Device) {
        .pc     = 0x0000, // TODO: where do we actually start?
        .sp     = 0xFF, // TODO: where does the sp init to?
        .acc    = 0x00,
        .x      = 0x00,
        .y      = 0x00,
        .status = 0x00,
    };

    memset(g_device.memory, 0x00, sizeof(g_device.memory[0])*MEMORY_SIZE_BYTES);
    memset(g_device.video, 0x00, sizeof(g_device.video[0])*VIDEO_BUFFER_SIZE_BYTES);
}

