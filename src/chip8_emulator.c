#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8_emulator.h"
#include "chip8_graphics.h"
#include "chip8_util.h"

#define MEMORY_SIZE 4096
#define PROG_START  512
#define PROG_SIZE   (MEMORY_SIZE - PROG_START)
#define STACK_SIZE  16
#define NUM_REGS    16
#define NUM_KEYS    16

// Container for memory, registers, flags, keyboard, etc.
struct emulator em;

// Built-in sprite management
static void setup_sprite_memory(void);
static uint16_t get_sprite_for(uint8_t sprite_val);

// Manage key inputs!
static void update_key_input(void);
static uint8_t get_next_hex_key(void);

// Handling various opcodes
static void process_leading_0(void);
static void process_leading_1(void);
static void process_leading_2(void);
static void process_leading_3(void);
static void process_leading_4(void);
static void process_leading_5(void);
static void process_leading_6(void);
static void process_leading_7(void);
static void process_leading_8(void);
static void process_leading_9(void);
static void process_leading_A(void);
static void process_leading_B(void);
static void process_leading_C(void);
static void process_leading_D(void);
static void process_leading_E(void);
static void process_leading_F(void);

void chip8_init(void)
{
    // Zero out all of our emulator's state
    memset(&em, 0, sizeof(em));

    // PC starts at 0x200
    em.PC = 0x200;

    setup_sprite_memory();
    graphics_init();
    graphics_draw_startup();
}

void chip8_load(const char *filename)
{
    FILE *prog_file = fopen(filename, "rb");
    if (prog_file == NULL) {
        printf("ERROR: Unable to open ROM file! Aborting...\n");
        exit(-1);
    }

    if (0 != fseek(prog_file, 0, SEEK_END)) {
        fclose(prog_file);
        graphics_deinit();
        printf("ERROR: Unable to determine ROM file size! Aborting...\n");
        exit(-1);
    }

    long int size = ftell(prog_file);

    fseek(prog_file, 0, SEEK_SET);

    if (size > PROG_SIZE) {
        fclose(prog_file);
        graphics_deinit();
        printf("ERROR: ROM size is larger than max capacity of %d bytes! Aborting...\n", PROG_SIZE);
        exit(-1);
    }

    if (size != fread(&em.memory[PROG_START], 1, size, prog_file)) {
        fclose(prog_file);
        graphics_deinit();
        printf("ERROR: Unable to read %ld bytes from file! Aborting...\n", size);
        exit(-1);
    }

    graphics_clear_screen();
    graphics_refresh_screen();
}

void chip8_emulate_cycle(void)
{
    em.opcode = em.memory[em.PC] << 8 | em.memory[em.PC + 1];

    switch (em.opcode & 0xF000) {
        case 0x0000:
            process_leading_0();
            break;
        case 0x1000:
            process_leading_1();
            break;
        case 0x2000:
            process_leading_2();
            break;
        case 0x3000:
            process_leading_3();
            break;
        case 0x4000:
            process_leading_4();
            break;
        case 0x5000:
            process_leading_5();
            break;
        case 0x6000:
            process_leading_6();
            break;
        case 0x7000:
            process_leading_7();
            break;
        case 0x8000:
            process_leading_8();
            break;
        case 0x9000:
            process_leading_9();
            break;
        case 0xA000:
            process_leading_A();
            break;
        case 0xB000:
            process_leading_B();
            break;
        case 0xC000:
            process_leading_C();
            break;
        case 0xD000:
            process_leading_D();
            break;
        case 0xE000:
            process_leading_E();
            break;
        case 0xF000:
            process_leading_F();
            break;
    }

    if (em.draw_flag) {
        em.draw_flag = 0;
        graphics_refresh_screen();
    }

    update_key_input();
}

void chip8_update_timers(void)
{
    if (em.delay > 0) {
        em.delay--;
    }

    if (em.sound > 0) {
        em.sound--;
    }
}

bool chip8_emulation_end_detected(void)
{
    return em.emulation_end_flag;
}

void chip8_deinit(void)
{
    graphics_deinit();
}

static void setup_sprite_memory(void)
{
    // Ripped from
    // https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    uint8_t chip8_fontset[80] =
    { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Sprite memory ends at 0x50 = address 80
    memcpy(em.memory, chip8_fontset, sizeof(chip8_fontset));
}

static uint16_t get_sprite_for(uint8_t sprite_val)
{
    if (sprite_val >= 0x10) {
        printf("ERROR: Cannot access built in sprites for non-hex characters!\n");
        return (uint16_t) -1;
    }

    return sprite_val * 5;
}

static void update_key_input(void)
{
    // 16 possible keys
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
        if (util_is_hex_key_pressed(i, false)) {
            em.key[i] = 1;

            // Also consume key
            util_get_char();

            em.key_fifo[em.key_fifo_write_ptr++] = i;
            em.key_fifo_write_ptr = util_constrain(em.key_fifo_write_ptr,
                                                   NUM_KEYS);
        }
    }
}

static uint8_t get_next_hex_key(void)
{
    uint8_t key;

    if (em.key_fifo_read_ptr != em.key_fifo_write_ptr) {
        key = em.key_fifo[em.key_fifo_read_ptr++];
        em.key_fifo_read_ptr = util_constrain(em.key_fifo_read_ptr, NUM_KEYS);
    } else {
        key = util_get_hex_key();
        if (key == (uint8_t)-1) {
            em.emulation_end_flag = 1;
        }
    }

    return key;
}

static void process_leading_0(void)
{
    switch(em.opcode & 0x00FF) {
        case 0x00E0:
            // 0x00E0 -> clear screen
            graphics_clear_screen();
            em.draw_flag = 1;
            em.PC += 2;
            break;
        case 0x00EE:
            // 0x00EE -> return from subroutine
            em.PC = em.stack[--em.SP];
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}

static void process_leading_1(void)
{
    em.PC = em.opcode & 0x0FFF;
}

static void process_leading_2(void)
{
    em.stack[em.SP++] = em.PC + 2;
    em.PC = em.opcode & 0x0FFF;
}

static void process_leading_3(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;
    if (em.V[reg] == (em.opcode & 0x00FF)) {
        em.PC += 4;
    } else {
        em.PC += 2;
    }
}

static void process_leading_4(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;
    if (em.V[reg] != (em.opcode & 0x00FF)) {
        em.PC += 4;
    } else {
        em.PC += 2;
    }
}

static void process_leading_5(void)
{
    if (em.opcode & 0x000F != 0) {
        printf("ERROR: Unrecognized opcode!\n");
        return;
    }

    uint8_t reg1 = (em.opcode & 0x0F00) >> 8;
    uint8_t reg2 = (em.opcode & 0x00F0) >> 4;
    if (em.V[reg1] == em.V[reg2]) {
        em.PC += 4;
    } else {
        em.PC += 2;
    }
}

static void process_leading_6(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;
    em.V[reg] = em.opcode & 0x00FF;
    em.PC += 2;
}

static void process_leading_7(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;
    em.V[reg] += em.opcode & 0x00FF;
    em.PC += 2;
}

static void process_leading_8(void)
{
    uint8_t reg1 = (em.opcode & 0x0F00) >> 8;
    uint8_t reg2 = (em.opcode & 0x00F0) >> 4;
    uint8_t vf_value;

    switch (em.opcode & 0x000F) {
        case 0x0000:
            em.V[reg1] = em.V[reg2];
            em.PC += 2;
            break;
        case 0x0001:
            em.V[reg1] = em.V[reg1] | em.V[reg2];
            em.PC += 2;
            break;
        case 0x0002:
            em.V[reg1] = em.V[reg1] & em.V[reg2];
            em.PC += 2;
            break;
        case 0x0003:
            em.V[reg1] = em.V[reg1] ^ em.V[reg2];
            em.PC += 2;
            break;
        case 0x0004:
            if (em.V[reg1] + em.V[reg2] < em.V[reg1]) {
                vf_value = 1;
            } else {
                vf_value = 0;
            }
            em.V[reg1] = em.V[reg1] + em.V[reg2];
            em.V[0xF] = vf_value;
            em.PC += 2;
            break;
        case 0x0005:
            if (em.V[reg1] >= em.V[reg2]) {
                vf_value = 1;
            } else {
                vf_value = 0;
            }
            em.V[reg1] = em.V[reg1] - em.V[reg2];
            em.V[0xF] = vf_value;
            em.PC += 2;
            break;
        case 0x0006:
#if 0
            em.V[0xF] = em.V[reg1] & 0x01;
            em.V[reg1] >>= 1;
#else
            em.V[0xF] = em.V[reg2] & 0x01;
            em.V[reg1] = em.V[reg2] >> 1;
#endif
            em.PC += 2;
            break;
        case 0x0007:
            if (em.V[reg2] >= em.V[reg1]) {
                vf_value = 1;
            } else {
                vf_value = 0;
            }
            em.V[reg1] = em.V[reg2] - em.V[reg1];
            em.V[0xF] = vf_value;
            em.PC += 2;
            break;
        case 0x000E:
#if 0
            em.V[0xF] = em.V[reg1] & 0x80;
            em.V[reg1] <<= 1;
#else
            em.V[0xF] = em.V[reg2] & 0x80;
            em.V[reg1] = em.V[reg2] << 1;
#endif
            em.PC += 2;
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}

static void process_leading_9(void)
{
    if ((em.opcode & 0x000F) != 0) {
        printf("ERROR: Unrecognized opcode!\n");
        return;
    }

    uint8_t reg1 = (em.opcode & 0x0F00) >> 8;
    uint8_t reg2 = (em.opcode & 0x00F0) >> 4;
    if (em.V[reg1] != em.V[reg2]) {
        em.PC += 4;
    } else {
        em.PC += 2;
    }
}

static void process_leading_A(void)
{
    em.I = em.opcode & 0x0FFF;
    em.PC += 2;
}

static void process_leading_B(void)
{
    em.PC = em.V[0] + (em.opcode & 0x0FFF);
}

static void process_leading_C(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;
    em.V[reg] = rand() & (em.opcode & 0x00FF);
    em.PC += 2;
}

static void process_leading_D(void)
{
    uint8_t reg1 = (em.opcode & 0x0F00) >> 8;
    uint8_t reg2 = (em.opcode & 0x00F0) >> 4;
    uint8_t n    = (em.opcode & 0x000F);
    // em.V[F] set if pixels flipped, which is return value of draw_sprite
    em.V[0xF] = graphics_draw_sprite(em.V[reg2], em.V[reg1], &em.memory[em.I], n);
    em.draw_flag = 1;
    em.PC += 2;
}

static void process_leading_E(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;

    switch (em.opcode & 0x00FF) {
        case 0x009E:
            em.PC += 2;
            if (em.key[em.V[reg]]) {
                em.PC += 2;
                em.key[em.V[reg]] = 0;
            }
            break;
        case 0x00A1:
            em.PC += 2;
            if (!em.key[em.V[reg]]) {
                em.PC += 2;
            } else {
                em.key[em.V[reg]] = 0;
            }
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}

static void process_leading_F(void)
{
    uint8_t reg = (em.opcode & 0x0F00) >> 8;

    switch (em.opcode & 0x00FF) {
        case 0x0007:
            em.V[reg] = em.delay;
            em.PC += 2;
            break;
        case 0x000A:
            em.V[reg] = get_next_hex_key();
            em.PC += 2;
            break;
        case 0x0015:
            em.delay = em.V[reg];
            em.PC += 2;
            break;
        case 0x0018:
            em.sound = em.V[reg];
            em.PC += 2;
            break;
        case 0x001E:
            em.I += em.V[reg];
            em.PC += 2;
            break;
        case 0x0029:
            em.I = get_sprite_for(em.V[reg]);
            em.PC += 2;
            break;
        case 0x0033:
            em.memory[em.I + 2] =   em.V[reg] % 10;
            em.memory[em.I + 1] = ((em.V[reg] % 100) - (em.V[reg] % 10)) / 10;
            em.memory[em.I]     =  (em.V[reg]        - (em.V[reg] % 100)) / 100;
            em.PC += 2;
            break;
        case 0x0055:
            memcpy(&em.memory[em.I], &em.V[0], reg + 1);
            em.PC += 2;
            break;
        case 0x0065:
            memcpy(&em.V[0], &em.memory[em.I], reg + 1);
            em.PC += 2;
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}
