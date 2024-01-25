#ifndef CATS_UTIL_H
#define CATS_UTIL_H

#define GET_BIT(byte, bit) ((byte & (1 << 7-bit)) != 0)
#define SET_BIT(byte, bit) (byte |= 1 << 7-bit)
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << 7-bit))

#endif