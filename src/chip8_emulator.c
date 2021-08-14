#include <stdint.h>

#include "chip8_emulator.h"
#include "chip8_graphics.h"
#include "chip8_util.h"

void chip8_init(void)
{
    graphics_init();
    graphics_draw_startup();
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

