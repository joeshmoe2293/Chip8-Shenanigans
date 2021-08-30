#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdbool.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define STACK_SIZE  16
#define NUM_REGS    16
#define NUM_KEYS    16

struct emulator {
    // RAM
    uint8_t  memory[MEMORY_SIZE];
    uint16_t stack[STACK_SIZE];

    // Registers
    uint8_t  V[NUM_REGS];
    uint16_t PC;
    uint16_t opcode;
    uint16_t I;
    uint8_t  delay;
    uint8_t  sound;
    uint8_t  SP;

    // Flags
    bool draw_flag;
    bool emulation_end_flag;
    bool single_step_flag;

    // Keyboard
    bool    key[NUM_KEYS];
    uint8_t key_fifo[NUM_KEYS];
    uint8_t key_fifo_read_ptr;
    uint8_t key_fifo_write_ptr;
};

void    util_delay_ms(uint8_t ms);
uint8_t util_constrain(uint8_t val, uint8_t max);
uint8_t util_get_char(void);
bool    util_is_key_pressed(uint8_t key, bool consume_key);
uint8_t util_get_hex_key(void);
bool    util_is_hex_key_pressed(uint8_t key, bool consume_key);

#endif
