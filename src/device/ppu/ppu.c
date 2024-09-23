#include "ppu.h"

#include "ppu_reg.h"
#include "color_palette.h"
#include "device/memory_map.h"
#include "helpers.h"

#include "log.h"

#include <string.h>

#define VIDEO_BUFFER_SIZE VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT

typedef struct __attribute__((__packed__)) {
    uint8_t pos_y;
    uint8_t tile_idx;
    uint8_t attr;
    uint8_t pos_x;
} OAMSprite;

#define OAM_SPRITE_COUNT 64
static OAMSprite s_oam[OAM_SPRITE_COUNT];

static uint32_t s_video_buffer[VIDEO_BUFFER_SIZE];

void ppu_init(void) {
    memset(s_video_buffer, 0, sizeof(s_video_buffer[0])*VIDEO_BUFFER_SIZE);

    ppu_reg_init();
}

void ppu_cycle(void) {
    // TODO
}


const uint32_t* ppu_get_buffer(void) {
    return s_video_buffer;
}

