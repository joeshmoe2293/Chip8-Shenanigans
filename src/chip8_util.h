#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdbool.h>
#include <stdint.h>

void util_delay_ms(uint8_t ms);
uint8_t util_constrain(uint8_t val, uint8_t max);
uint8_t util_get_char(void);
bool util_is_key_pressed(uint8_t key, bool consume_key);
uint8_t util_get_hex_key(void);
bool util_is_hex_key_pressed(uint8_t key, bool consume_key);

#endif
