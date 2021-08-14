#include <ncurses.h>
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
