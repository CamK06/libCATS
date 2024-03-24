#ifndef CATS_INTERLEAVER_H
#define CATS_INTERLEAVER_H

#include <stdint.h>
#include <stddef.h>

void cats_interleave(uint8_t* data, size_t len);
void cats_deinterleave(uint8_t* data, size_t len);

#endif
