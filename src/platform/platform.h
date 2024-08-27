#ifndef PLATFORM_H
#define PLATFORM_H

int platform_init(void);
void platform_cleanup(void);
void platform_poll_events(void);
int platform_is_running(void);
void platform_draw(void);

#endif

