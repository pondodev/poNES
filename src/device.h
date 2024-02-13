#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#define MEMORY_SIZE_BYTES       0xFFFF
#define VIDEO_BUFFER_WIDTH      256
#define VIDEO_BUFFER_HEIGHT     240
#define VIDEO_BUFFER_SIZE_BYTES VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT

#define ZERO_PAGE_START_ADDR    0x0000
#define ZERO_PAGE_END_ADDR      0x00FF
#define STACK_PAGE_START_ADDR   0x0100
#define STACK_PAGE_END_ADDR     0x01FF

typedef enum {
    FLAG_CARRY          = 1 << 0,
    FLAG_ZERO           = 1 << 1,
    FLAG_IRQ_DISABLE    = 1 << 2,
    FLAG_DEC_MODE       = 1 << 3,
    FLAG_BREAK_CMD      = 1 << 4,
    FLAG_OVERFLOW       = 1 << 5,
    FLAG_NEGATIVE       = 1 << 6,
} CPUStatusFlags;

typedef struct {
    // registers
    uint16_t    pc;
    uint8_t     sp;
    uint8_t     acc;
    uint8_t     x;
    uint8_t     y;
    uint8_t     status;

    // buffers
    uint8_t     memory[MEMORY_SIZE_BYTES];
    uint8_t     video[VIDEO_BUFFER_SIZE_BYTES];
} Device;

extern Device g_device;

void device_init(void);

#endif

