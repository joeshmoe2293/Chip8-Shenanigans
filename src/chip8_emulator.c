#include <stdint.h>

#include "chip8_emulator.h"
#include "chip8_graphics.h"
#include "chip8_util.h"

// For initializing screen with any startup messages
static void em_init_graphics(void);

void chip8_init(void)
{
    em_init_graphics();
}

void chip8_load(const char *filename)
{

}

void chip8_emulate_cycle(void)
{

}

void chip8_deinit(void)
{
    graphics_deinit();
}

static void em_init_graphics(void)
{
    graphics_init();
    
    uint8_t chip8_startup_sprite[15] = {
        0x6A, 0x8A, 0x8E, 0x8A, 0x6A, 0xEE, 0x4A, 0x4E,
        0x48, 0xE8, 0xE0, 0xA0, 0xE0, 0xA0, 0xE0
    };

    uint8_t row = 13;
    uint8_t col = 20;

    // Draw CH -> IP -> 8
    for (int c = 0, r = 0; c < 24; c += 8, r += 5) {
        // c -> move forward columns,
        // r -> move forward rows in sprite bytes to get next letter
        graphics_draw_sprite(row, col + c, chip8_startup_sprite + r, 5);
    }

    // Top and bottom border
    for (row = 0; row < 32; row += 31) {
        for (col = 0; col < 64; col++) {
            graphics_toggle_pixel(row, col);
        }
    }

    for (col = 0; col < 64; col += 63) {
        for (row = 1; row < 31; row++) {
            graphics_toggle_pixel(row, col);
        }
    }

    graphics_refresh_screen();
}
