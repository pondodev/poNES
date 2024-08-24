#include "device.h"

#include "instructions.h"
#include "memory.h"
#include "ppu.h"

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
        .status = 0x00 & kCPUSTATUSFLAG_IRQ_DISABLE,
        .cart   = NULL,
    };

    memory_init();
    ppu_init();
    instr_init();
}

void device_load_cart(Cart* cart) {
    g_device.cart = cart;

    const void* first_bank = g_device.cart->buffer + g_device.cart->prg_rom_start;
    memory_load_cart_rom_bank(first_bank, g_device.cart->prg_rom_size);
    g_device.pc = cart_entrypoint(g_device.cart);
}

void device_exec(void) {
    InstrInfo instr = instr_decode();
    instr_exec(&instr);

    g_device.pc += instr.stride;
}

