#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8_graphics.h"
#include "chip8_util.h"

#define ROW_COUNT 32
#define COL_COUNT 64

static uint8_t screen[ROW_COUNT][COL_COUNT];

static void init_colors(void);
static void set_pixel(bool pixel_on);

void graphics_init(void)
{
    initscr();
    init_colors();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
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
    graphics_refresh_screen();
}

void graphics_draw_sprite(uint8_t row, uint8_t col,
                          uint8_t *sprite, uint8_t num_bytes)
{
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
                graphics_toggle_pixel(util_constrain(row + r, ROW_COUNT),
                                      util_constrain(col + c, COL_COUNT));

#if 1
                graphics_refresh_screen();
#endif
            }
        }
    }
}

void graphics_deinit(void)
{
    endwin();
}

static void init_colors(void)
{
    if (has_colors()) {
        if (start_color() == OK) {
            init_pair(1, COLOR_WHITE, COLOR_WHITE);
            init_pair(2, COLOR_BLUE, COLOR_BLUE);
        }
    } else {
        printf("ERROR: need colors atm!\n");
        exit(-1);
    }
}

static void set_pixel(bool pixel_on)
{
    if (pixel_on) {
        attroff(COLOR_PAIR(2));
        attrset(COLOR_PAIR(1));
    } else {
        attroff(COLOR_PAIR(1));
        attrset(COLOR_PAIR(2));
    }

    // Two spaces -> more square-like
    addch(' ');
    addch(' ');
}
