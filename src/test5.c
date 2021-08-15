#include "chip8_emulator.h"
#include "chip8_util.h"

int main(int argc, char *argv[])
{
    chip8_init();
    util_get_char();

    if (argc > 1) {
        chip8_load(argv[1]);

        for (uint32_t cycle = 0; ; cycle++) {
            chip8_emulate_cycle();

            util_delay_ms(1);

            if ((cycle + 1) % 17 == 0) {
                chip8_update_timers();
            }

            if (util_is_key_pressed('k', true)) {
                break;
            }
        }
    }

    chip8_deinit();

    return 0;
}

