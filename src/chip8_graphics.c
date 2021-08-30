#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8_graphics.h"
#include "chip8_util.h"

#define ROW_COUNT        32
#define COL_COUNT        64
#define SPACES_PER_PIXEL 2

static uint8_t screen[ROW_COUNT][COL_COUNT];

static void init_colors(void);
static void set_pixel(bool pixel_on);
static void draw_word_sprite(uint8_t row, uint8_t col,
                             uint8_t *sprite, uint8_t num_letters);
static void clear_debug_row(uint8_t row, uint8_t num_rows);

void graphics_init(void)
{
    initscr();
    init_colors();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    memset(screen, 0, sizeof(screen));
}

void graphics_toggle_pixel(uint8_t row, uint8_t col)
{
    screen[row][col] ^= 1;
}

void graphics_refresh_screen(void)
{
    for (int row = 0; row < ROW_COUNT; row++) {
        move(row, 0);

        for (int col = 0; col < COL_COUNT; col++) {
            set_pixel(screen[row][col]);
        }
    }

    refresh();
}

void graphics_clear_screen(void)
{
    memset(screen, 0, sizeof(screen));
}

bool graphics_draw_sprite(uint8_t row, uint8_t col,
                          uint8_t *sprite, uint8_t num_bytes)
{
    bool pixels_flipped = false;
    row = util_constrain(row, ROW_COUNT);
    col = util_constrain(col, COL_COUNT);

    for (int r = 0; r < num_bytes; r++) {
        if (sprite[r] == 0x00) {
            // No pixels to draw, can skip
            continue;
        }

        // 8 bits per row
        for (int c = 0; c < 8; c++) {
            // Check bits from left to right (high to low)
            if (sprite[r] & (1 << (7 - c))) {
                uint8_t r_const = util_constrain(row + r, ROW_COUNT);
                uint8_t c_const = util_constrain(col + c, COL_COUNT);
                graphics_toggle_pixel(r_const, c_const);

                if (screen[r_const][c_const] == 0) {
                    pixels_flipped = true;
                }

#if defined(SPRITE_DEBUG)
                graphics_refresh_screen();
#endif
            }
        }
    }

    return pixels_flipped;
}

void graphics_draw_startup(void)
{
    uint8_t chip8_sprite[15] = {
        0x6A, 0x8A, 0x8E, 0x8A, 0x6A, 0xEE, 0x4A, 0x4E,
        0x48, 0xE8, 0xE0, 0xA0, 0xE0, 0xA0, 0xE0
    };

    uint8_t press_sprite[15] = {
        0xEC, 0xAA, 0xEC, 0x8A, 0x8A, 0xEE, 0x88, 0xEE,
        0x82, 0xEE, 0xE0, 0x80, 0xE0, 0x20, 0xE0
    };

    uint8_t any_sprite[10] = {
        0x49, 0xAD, 0xEB, 0xA9, 0xA9,
        0x44, 0x28, 0x10, 0x10, 0x10
    };

    uint8_t key_sprite[10] = {
        0x97, 0xA4, 0xC7, 0xA4, 0x97,
        0x44, 0x28, 0x10, 0x10, 0x10
    };

    uint8_t row = 10;
    uint8_t col = 20;

    // CHIP8
    draw_word_sprite(row, col, chip8_sprite, 5);

    // PRESS
    row += 8;
    col = 5;
    draw_word_sprite(row, col, press_sprite, 5);

    // ANY
    col += 21;
    draw_word_sprite(row, col, any_sprite, 3);

    // KEY
    col += 16;
    draw_word_sprite(row, col, key_sprite, 3);

    // Top and bottom border
    for (row = 0; row < 32; row += 31) {
        for (col = 0; col < 64; col++) {
            graphics_toggle_pixel(row, col);
        }
    }

    // Side border
    for (col = 0; col < 64; col += 63) {
        for (row = 1; row < 31; row++) {
            graphics_toggle_pixel(row, col);
        }
    }

    graphics_refresh_screen();
}

void graphics_draw_program_state(struct emulator *em)
{
    // Clear any previous debug text in debug window
    clear_debug_row(ROW_COUNT, 32);

    // Move below output window
    move(ROW_COUNT, 0);

    attrset(COLOR_PAIR(3));

    // PC, opcode, memory register, delay, sound, SP
    printw("PC: 0x%03X\tI: 0x%03X\t"
           "Delay: %d\tSound: %d\tSP: 0x%X\n",
           em->PC, em->I, em->delay, em->sound, em->SP);

    // Program registers
    for (int i = 0; i < NUM_REGS; i++) {
        printw("V[%X]: 0x%02X\t", i, em->V[i]);

        // Add a newline every 8 registers
        if ((i + 1) % 8 == 0) {
            addch('\n');
        }
    }

    // Memory
    printw("Memory:\n");
    for (uint16_t addr = em->PC - 4; addr < em->PC + 10; addr += 2) {
        // Print next instruction in bold
        if (addr == em->PC) {
            attrset(A_BOLD);
        }

        // Don't print if beyond edge of memory
        if (addr > 0 && addr < MEMORY_SIZE) {
            printw("0x%03X: %02X %02X\t",
                   addr, em->memory[addr], em->memory[addr + 1]);
        }

        // Disable bold printing
        if (addr == em->PC) {
            attroff(A_BOLD);
        }
    }

    addch('\n');

    // Stack trace
    for (int i = 0; i < em->SP; i++) {
        // Don't print if beyond stack's end
        if (i < STACK_SIZE) {
            printw("Stack[0x%X]: 0x%03X\t", i, em->stack[i]);
        }

        if ((i + 1) % 4 == 0) {
            addch('\n');
        }
    }

    addch('\n');

    attroff(COLOR_PAIR(3));
}

void graphics_clear_program_state(void)
{
    // Clear all debug info
    clear_debug_row(ROW_COUNT, 32);

    // Move outside output window
    move(ROW_COUNT, 0);

    // Print resuming then overwrite other info
    attrset(COLOR_PAIR(3));

    printw("Resuming...");

    attroff(COLOR_PAIR(3));
}

void graphics_deinit(void)
{
    endwin();
}

static void init_colors(void)
{
    if (has_colors()) {
        if (start_color() == OK) {
            // Pixel = 1
            init_pair(1, COLOR_WHITE, COLOR_WHITE);
            // Pixel = 0
            init_pair(2, COLOR_BLUE, COLOR_BLUE);
            // Debug info
            init_pair(3, COLOR_WHITE, COLOR_BLACK);
            // Cleared info
            init_pair(4, COLOR_BLACK, COLOR_BLACK);
        }
    } else {
        printf("ERROR: need colors atm!\n");
        exit(-1);
    }
}

static void set_pixel(bool pixel_on)
{
    // Set right color
    if (pixel_on) {
        attrset(COLOR_PAIR(1));
    } else {
        attrset(COLOR_PAIR(2));
    }

    // Two spaces -> more square-like
    for (int i = 0; i < SPACES_PER_PIXEL; i++) {
        addch(' ');
    }

    // Clear for setting next pixels
    if (pixel_on) {
        attroff(COLOR_PAIR(1));
    } else {
        attroff(COLOR_PAIR(2));
    }
}

static void draw_word_sprite(uint8_t row, uint8_t col,
                             uint8_t *sprite, uint8_t num_letters)
{
    // 8 cols per letter pair, #rows = (#letters + 1) / 2
    for (int c = 0, r = 0; c < 8 * (num_letters + 1) / 2; c += 8, r += 5) {
        // c -> move forward columns,
        // r -> move forward rows in sprite bytes to get next letter
        graphics_draw_sprite(row, col + c, sprite + r, 5);
    }
}

static void clear_debug_row(uint8_t row, uint8_t num_rows)
{
    attrset(COLOR_PAIR(4));

    for (int r = row; r < row + num_rows; r++) {
        move(r, 0);

        // Overwrite debug text with black spaces
        for (int c = 0; c < COL_COUNT * SPACES_PER_PIXEL; c++) {
            addch(' ');
        }
    }

    attroff(COLOR_PAIR(4));
}
