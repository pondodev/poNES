#include "device.h"

#include "instructions.h"

#include "raylib.h"

#include <string.h>

typedef enum {
    kSTATUSFLAG_CARRY       = 1 << 0,
    kSTATUSFLAG_ZERO        = 1 << 1,
    kSTATUSFLAG_IRQ_DISABLE = 1 << 2,
    kSTATUSFLAG_DECIMAL     = 1 << 3,
    kSTATUSFLAG_OVERFLOW    = 1 << 6,
    kSTATUSFLAG_NEGATIVE    = 1 << 6,
} StatusFlag;

Device g_device;

void device_init(Cart* cart) {
    // initial values based on https://www.nesdev.org/wiki/CPU_power_up_state
    g_device = (Device) {
        .pc     = 0xFFFC,
        .sp     = 0xFD,
        .acc    = 0x00,
        .x      = 0x00,
        .y      = 0x00,
        .status = 0x00 & kSTATUSFLAG_IRQ_DISABLE,
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

