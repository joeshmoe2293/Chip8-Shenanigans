#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>

#include "chip8_util.h"

void util_delay_ms(uint8_t ms)
{
    napms(ms);
}

uint8_t util_constrain(uint8_t val, uint8_t max)
{
    return (val % max);
}

uint8_t util_get_char(void)
{
    return (uint8_t)getch();
}

uint8_t util_is_key_pressed(uint8_t key, bool consume_key)
{
    nodelay(stdscr, TRUE);
    uint8_t pressed = (uint8_t)getch();
    nodelay(stdscr, FALSE);

    if (pressed != ERR) {
        if (!consume_key) {
            ungetch(pressed);
        }
    }

    return pressed != ERR && key == pressed;
}
