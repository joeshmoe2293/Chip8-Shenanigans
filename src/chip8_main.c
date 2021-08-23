#include <stdbool.h>

#include "chip8_emulator.h"
#include "chip8_util.h"

int main(int argc, char *argv[])
{
    chip8_init();
    util_get_char();

    if (argc > 1) {
        chip8_load(argv[1]);

        bool in_single_step = false;

        for (uint32_t cycle = 0; ; cycle++) {
            if (in_single_step) {
                chip8_display_program_status();

                // Wait until step (i) / resume (p) / end (k)
                // pressed before continuing
                uint8_t key;
                do {
                    key = util_get_char();
                } while (key != 'k' && key != 'p' && key != 'i');

                if (key == 'k') {
                    break;
                } else if (key == 'p') {
                    in_single_step = false;
                    chip8_clear_program_status();
                } // else key == i -> single step continue
            }

            chip8_emulate_cycle();

            util_delay_ms(1);

            if ((cycle + 1) % 17 == 0) {
                chip8_update_timers();
            }

            if (util_is_key_pressed('k', false) ||
                chip8_emulation_end_detected()) {
                break;
            } else if (util_is_key_pressed('p', true) ||
                       chip8_single_step_detected()) {
                in_single_step = true;
            }
        }
    }

    chip8_deinit();

    return 0;
}

