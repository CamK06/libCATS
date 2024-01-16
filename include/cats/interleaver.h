#ifndef CATS_INTERLEAVER_H
#define CATS_INTERLEAVER_H

#include <stdint.h>

// 32-block interleaver
// data is both input and output
void cats_interleave(uint8_t* data, int len);
// 32-block deinterleaver
// data is both input and output
void cats_deinterleave(uint8_t* data, int len);

#endif
