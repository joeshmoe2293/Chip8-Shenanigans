#include "chip8_emulator.h"
#include "chip8_util.h"

int main()
{
    chip8_init();
    util_get_char();
    chip8_deinit();

    return 0;
}

