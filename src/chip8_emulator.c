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

// RAM
static uint8_t  memory[MEMORY_SIZE];
static uint16_t stack[STACK_SIZE];

// Registers
static uint8_t  V[NUM_REGS];
static uint8_t  I;
static uint8_t  delay;
static uint8_t  sound;
static uint16_t PC;
static uint16_t opcode;
static uint8_t  SP;
static bool     draw_flag;     

// Keyboard
static uint8_t key[NUM_KEYS];

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
    graphics_init();
    graphics_draw_startup();

    // Memory
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));

    // Registers
    memset(V, 0, sizeof(V));
    I = 0;
    delay = 0;
    sound = 0;
    PC = 0x200;
    opcode = 0;
    SP = 0;
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

    if (size != fread(&memory[PROG_START], 1, size, prog_file)) {
        fclose(prog_file);
        graphics_deinit();
        printf("ERROR: Unable to read %ld bytes from file! Aborting...\n", size);
        exit(-1);
    }
}

void chip8_emulate_cycle(void)
{
    opcode = memory[PC] << 8 | memory[PC + 1];

    switch (opcode & 0xF000) {
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

    if (draw_flag) {
        draw_flag = 0;
        graphics_refresh_screen();
    }
}

void chip8_deinit(void)
{
    graphics_deinit();
}

static void process_leading_0(void)
{
    switch(opcode & 0x00FF) {
        case 0x00E0:
            // 0x00E0 -> clear screen
            graphics_clear_screen();
            draw_flag = 1;
            PC += 2;
            break;
        case 0x00EE:
            // 0x00EE -> return from subroutine
            PC = stack[SP--];
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}

static void process_leading_1(void)
{
    PC = opcode & 0x0FFF;
}

static void process_leading_2(void)
{
    stack[SP++] = PC + 2;
    PC = opcode & 0x0FFF;
}

static void process_leading_3(void)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    if (V[reg] == opcode & 0x00FF) {
        PC += 4;
    } else {
        PC += 2;
    }
}

static void process_leading_4(void)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    if (V[reg] != opcode & 0x00FF) {
        PC += 4;
    } else {
        PC += 2;
    }
}

static void process_leading_5(void)
{
    if (opcode & 0x000F != 0) {
        printf("ERROR: Unrecognized opcode!\n");
        return;
    }

    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    if (V[reg1] == V[reg2]) {
        PC += 4;
    } else {
        PC += 2;
    }
}

static void process_leading_6(void)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    V[reg] = opcode & 0x00FF;
    PC += 2;
}

static void process_leading_7(void)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    V[reg] += opcode & 0x00FF;
    PC += 2;
}

static void process_leading_8(void)
{
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F) {
        case 0x0000:
            V[reg1] = V[reg2];
            PC += 2;
            break;
        case 0x0001:
            V[reg1] = V[reg1] | V[reg2];
            PC += 2;
            break;
        case 0x0002:
            V[reg1] = V[reg1] & V[reg2];
            PC += 2;
            break;
        case 0x0003:
            V[reg1] = V[reg1] ^ V[reg2];
            PC += 2;
            break;
        case 0x0004:
            if (V[reg1] + V[reg2] < V[reg1]) {
                V[0xF] = 1;
            } else {
                V[0xF] = 0;
            }
            V[reg1] = V[reg1] + V[reg2];
            PC += 2;
            break;
        case 0x0005:
            if (V[reg1] - V[reg2] > V[reg1]) {
                V[0xF] = 1;
            } else {
                V[0xF] = 0;
            }
            V[reg1] = V[reg1] - V[reg2];
            PC += 2;
            break;
        case 0x0006:
            V[0xF] = V[reg1] & 0x01;
            V[reg1] >>= 1;
            PC += 2;
            break;
        case 0x0007:
            if (V[reg2] - V[reg1] > V[reg2]) {
                V[0xF] = 1;
            } else {
                V[0xF] = 0;
            }
            V[reg2] = V[reg2] - V[reg1];
            PC += 2;
            break;
        case 0x000E:
            V[0xF] = V[reg1] & 0x80;
            V[reg1] <<= 1;
            PC += 2;
            break;
        default:
            printf("ERROR: Unrecognized opcode!\n");
            break;
    }
}

static void process_leading_9(void)
{
    
}

static void process_leading_A(void)
{
    
}

static void process_leading_B(void)
{
    
}

static void process_leading_C(void)
{
    
}

static void process_leading_D(void)
{
    
}

static void process_leading_E(void)
{
    
}

static void process_leading_F(void)
{
    
}
