#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include <stdbool.h>

void chip8_init(void);
void chip8_load(const char *filename);
void chip8_emulate_cycle(void);
void chip8_update_timers(void);
bool chip8_emulation_end_detected(void);
void chip8_deinit(void);

#endif
