#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdint.h>

void util_delay_ms(uint8_t ms);
uint8_t util_constrain(uint8_t val, uint8_t max);
uint8_t util_get_char(void);

#endif
