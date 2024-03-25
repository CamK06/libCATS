#include "cats/util.h"

#include <stdint.h>

// Functions I was mostly too lazy and tired to research and write, so ChatGPT wrote them

uint16_t float32_to_float16(float value) {
    uint32_t f32 = *(uint32_t*)&value;
    uint32_t sign = (f32 >> 16) & 0x8000;
    uint32_t exponent = ((f32 >> 23) - 127 + 15) & 0x1F;
    uint32_t mantissa = f32 & 0x007FFFFF;
    uint32_t f16 = sign | (exponent << 10) | (mantissa >> 13);
    return (uint16_t)f16;
}

float float16_to_float32(uint16_t value) {
    uint32_t f16 = value;
    uint32_t sign = (f16 & 0x8000) << 16;
    uint32_t exponent = ((f16 >> 10) & 0x1F) + 127 - 15;
    uint32_t mantissa = (f16 & 0x03FF) << 13;
    uint32_t f32 = sign | (exponent << 23) | mantissa;
    return *(float*)&f32;
}

void uint64_to_5byte(uint64_t value, uint8_t* result) {
    result[0] = (value >> 32) & 0xFF;
    result[1] = (value >> 24) & 0xFF;
    result[2] = (value >> 16) & 0xFF;
    result[3] = (value >> 8) & 0xFF;
    result[4] = value & 0xFF;
}

int32_t lat_to_int32(double lat)
{
    return -(int32_t)(lat * (float)(1 << 31) / 90);
}

int32_t lon_to_int32(double lon)
{
    return -(int32_t)(lon * (float)(1 << 31) / 180);
}

double int32_to_lat(int32_t lat)
{
    return -(double)lat / (float)(1 << 31) * 90;
}

double int32_to_lon(int32_t lon)
{
    return -(double)lon / (float)(1 << 31) * 180;
}

int16_t lat_to_int16(double lat)
{
    return -(int16_t)(lat * (float)(1 << 15) / 90);
}

int16_t lon_to_int16(double lon)
{
    return -(int16_t)(lon * (float)(1 << 15) / 180);
}

double int16_to_lat(int16_t lat)
{
    return -(double)lat / (float)(1 << 15) * 90;
}

double int16_to_lon(int16_t lon)
{
    return -(double)lon / (float)(1 << 15) * 180;
}

uint8_t cats_rssi_encode(int16_t rssi)
{
    return (1.5 * rssi) + 240;
}

int16_t cats_rssi_decode(uint8_t rssi)
{
    return (rssi - 240) / 1.5;
}