#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
static uint8_t  SP;

// Keyboard
static uint8_t key[NUM_KEYS];

void chip8_init(void)
{
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

    if (size != fread(&memory[PROG_START], 1, size, prog_file)) {
        fclose(prog_file);
        graphics_deinit();
        printf("ERROR: Unable to read %ld bytes from file! Aborting...\n", size);
        exit(-1);
    }
}

void chip8_emulate_cycle(void)
{

}

void chip8_deinit(void)
{
    graphics_deinit();
}

