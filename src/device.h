#ifndef DEVICE_H
#define DEVICE_H

#include "cart.h"

#include <stdint.h>

#define VIDEO_BUFFER_WIDTH      256
#define VIDEO_BUFFER_HEIGHT     240
#define VIDEO_BUFFER_SIZE_BYTES VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT

typedef enum {
    kCPUSTATUSFLAG_CARRY        = 1 << 0,
    kCPUSTATUSFLAG_ZERO         = 1 << 1,
    kCPUSTATUSFLAG_IRQ_DISABLE  = 1 << 2,
    kCPUSTATUSFLAG_DEC_MODE     = 1 << 3,
    kCPUSTATUSFLAG_BREAK_CMD    = 1 << 4,
    kCPUSTATUSFLAG_OVERFLOW     = 1 << 5,
    kCPUSTATUSFLAG_NEGATIVE     = 1 << 6,
} CPUStatusFlag;

typedef struct {
    // registers
    uint16_t    pc;
    uint8_t     sp;
    uint8_t     acc;
    uint8_t     x;
    uint8_t     y;
    uint8_t     status;

    // buffers
    uint8_t     video[VIDEO_BUFFER_SIZE_BYTES];

    Cart*       cart;
} Device;

extern Device g_device;

void device_init(void);
void device_load_cart(Cart* cart);
void device_exec(void);
uint8_t device_read_mem(uint16_t addr);

#endif

