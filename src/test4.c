#include "chip8_emulator.h"
#include "chip8_util.h"

int main(int argc, char *argv[])
{
    chip8_init();
    util_get_char();

    if (argc > 1) {
        chip8_load(argv[1]);
    }
    chip8_deinit();

    return 0;
}

