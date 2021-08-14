#include "chip8_graphics.h"
#include "chip8_util.h"

int main()
{
    graphics_init();

    uint8_t sprite[5] = {
        0x24, 0x00, 0x00, 0x81, 0x7E
    };

    for (int row = 0; row < 32; row++) {
        for (int col = 0; col < 64; col++) {
            if ((row + col) % 2 == 0) {
                graphics_toggle_pixel(row, col);
            }

            util_delay_ms(1);

            graphics_refresh_screen();
        }
    }

    util_get_char();

    graphics_clear_screen();
    graphics_draw_sprite(13, 28, sprite, sizeof(sprite));
    graphics_refresh_screen();
    util_get_char();

    graphics_deinit();

    return 0;
}

