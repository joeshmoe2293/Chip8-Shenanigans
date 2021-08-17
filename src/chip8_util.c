#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>

#include "chip8_util.h"

/* Chip-8 Key  Keyboard
 * ----------  ---------
 *    1 2 3 C    1 2 3 4
 *    4 5 6 D    q w e r
 *    7 8 9 E    a s d f
 *    A 0 B F    z x c v
 * */
static const uint8_t chip8_valid_keys[16] = {
    'x', '1', '2', '3',
    'q', 'w', 'e', 'a',
    's', 'd', 'z', 'c',
    '4', 'r', 'f', 'v'
};


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

bool util_is_key_pressed(uint8_t key, bool consume_key)
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

uint8_t util_get_hex_key(void)
{
    bool valid_key = false;
    uint8_t pressed_key;

    do {
        pressed_key = util_get_char();

        if (pressed_key == 'k') {
            pressed_key = (uint8_t)-1;
            break;
        }

        for (uint8_t i = 0; i < sizeof(chip8_valid_keys); i++) {
            if (pressed_key == chip8_valid_keys[i]) {
                valid_key = true;
                pressed_key = i;
                break;
            }
        }
    } while (!valid_key);

    return pressed_key;
}

bool util_is_hex_key_pressed(uint8_t key, bool consume_key)
{
    return util_is_key_pressed(chip8_valid_keys[key], false);
}
