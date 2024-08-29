#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef enum {
    kINPUT_UP      = 0,
    kINPUT_DOWN    = 1,
    kINPUT_LEFT    = 2,
    kINPUT_RIGHT   = 3,
    kINPUT_A       = 4,
    kINPUT_B       = 5,
    kINPUT_START   = 6,
    kINPUT_SELECT  = 7,
    kINPUT_SIZE,
} Input;

typedef uint8_t InputFlags;

int platform_init(void);
void platform_cleanup(void);
void platform_poll_events(void);
InputFlags platform_get_inputs(void);
int platform_is_running(void);
void platform_update_frame_buffer(uint32_t* buffer);
void platform_draw(void);

#endif

