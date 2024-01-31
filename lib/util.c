#include "cats/util.h"
#include <stdint.h>

// Functions I was too lazy and tired to research and write, so ChatGPT wrote them

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