#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

void graphics_init(void);
void graphics_toggle_pixel(uint8_t row, uint8_t col);
void graphics_refresh_screen(void);
void graphics_clear_screen(void);
bool graphics_draw_sprite(uint8_t row, uint8_t col,
                          uint8_t *sprite, uint8_t num_bytes);
void graphics_draw_startup(void);
void graphics_deinit(void);

#endif
