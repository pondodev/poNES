#include "mapper.h"

#include "log.h"

CartMapper mapper_get_type(uint16_t mapper_num) {
    switch (mapper_num) {
        case 0:
            log_info("mapper type NROM");
            return kCARTMAPPER_NROM;
        default:
            log_info("unknown mapper type '%u'", mapper_num);
            return kCARTMAPPER_UNKNOWN;
    }
}

uint16_t mapper_get_start_addr(CartMapper mapper) {
    switch (mapper) {
        case kCARTMAPPER_NROM:      return 0x8000;
        case kCARTMAPPER_UNKNOWN:   return 0;
    }
}
