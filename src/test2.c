#include "chip8_graphics.h"
#include "chip8_util.h"

int main()
{
    graphics_init();

    uint8_t sprite[5] = {
        0x24, 0x00, 0x00, 0x81, 0x7E
    };

    uint8_t row = 13;
    uint8_t col = 58;

    graphics_clear_screen();
    graphics_draw_sprite(row, col, sprite, sizeof(sprite));
    graphics_refresh_screen();
    util_get_char();

    while (!util_is_key_pressed('q', true)) {
        graphics_clear_screen();
        graphics_draw_sprite(++row, ++col, sprite, sizeof(sprite));
        graphics_refresh_screen();
        util_delay_ms(100);
    }

    graphics_deinit();

    return 0;
}

