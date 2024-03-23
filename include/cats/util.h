#ifndef CATS_UTIL_H
#define CATS_UTIL_H

#include <stdint.h>

#define GET_BIT(byte, bit) (((byte) & (1 << (7-(bit)))) != 0)
#define SET_BIT(byte, bit) (byte |= 1 << 7-bit)
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << 7-bit))
#define FLIP_BIT(byte, bit) (byte ^= (1 << (7 - bit)))

uint16_t float32_to_float16(float value);
float float16_to_float32(uint16_t value);

int32_t lat_to_int32(double lat);
int32_t lon_to_int32(double lon);
double int32_to_lat(int32_t lat);
double int32_to_lon(int32_t lon);

int16_t lat_to_int16(double lat);
int16_t lon_to_int16(double lon);
double int16_to_lat(int16_t lat);
double int16_to_lon(int16_t lon);

#endif