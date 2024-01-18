#ifndef CATS_WHITENER_H
#define CATS_WHITENER_H

#include <stdint.h>

#define LFSR_START_STATE 0xE9CF

uint8_t cats_lfsr(uint16_t* state);
uint8_t cats_lfsr_byte(uint16_t* state);
void cats_whiten(uint8_t* data, int len);

#endif
