#ifndef CATS_UTIL_H
#define CATS_UTIL_H

#include <stdint.h>

#define GET_BIT(byte, bit) ((byte & (1 << 7-bit)) != 0)
#define SET_BIT(byte, bit) (byte |= 1 << 7-bit)
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << 7-bit))
#define FLIP_BIT(byte, bit) (byte ^= (1 << (7 - bit)))

uint16_t float32_to_float16(float value);
float float16_to_float32(uint16_t value);

#endif