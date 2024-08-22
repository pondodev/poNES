#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

typedef enum {
    kCARTMAPPER_NROM        = 0,
    kCARTMAPPER_UNKNOWN,
} CartMapper;

CartMapper mapper_get_type(uint16_t mapper_num);
uint16_t mapper_get_start_addr(CartMapper mapper);

#endif

