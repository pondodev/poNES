#include "device.h"

#include "cpu.h"
#include "ppu.h"
#include "ram.h"
#include "helpers.h"

#include <string.h>

Device g_device;

void device_init(void) {
    // initial values based on https://www.nesdev.org/wiki/CPU_power_up_state
    g_device = (Device) {
        .pc     = 0xFFFC,
        .sp     = 0xFD,
        .acc    = 0x00,
        .x      = 0x00,
        .y      = 0x00,
        .status = 0x00 & BIT(kCPUSTATUSFLAG_IRQ_DISABLE),
        .cart   = NULL,
    };

    cpu_init();
    ppu_init();
    ram_init();
}

void device_load_cart(Cart* cart) {
    g_device.cart = cart;
}

void device_exec(void) {
    const InstrInfo instr = cpu_decode();
    cpu_exec(&instr);

    g_device.pc += instr.stride;
}

