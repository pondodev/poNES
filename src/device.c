#include "device.h"

#include "instructions.h"

#include "raylib.h"

#include <string.h>

Device g_device;

void device_init(Cart* cart) {
    g_device = (Device) {
        .pc     = cart->start_addr,
        .sp     = 0xFF, // TODO: where does the sp init to?
        .acc    = 0x00,
        .x      = 0x00,
        .y      = 0x00,
        .status = 0x00,
        .cart   = cart,
    };

    memset(g_device.memory, 0x00, sizeof(g_device.memory[0])*MEMORY_SIZE_BYTES);
    memset(g_device.video, 0x00, sizeof(g_device.video[0])*VIDEO_BUFFER_SIZE_BYTES);

    instr_init();
}

void device_exec(void) {
    InstrInfo instr = instr_decode();
    instr_exec(&instr);

    g_device.pc += instr.stride;
}


