#ifndef CATS_INTERLEAVER_H
#define CATS_INTERLEAVER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Interleave the supplied data as per the CATS standard
 *
 * @param data Input and output buffer
 * @param len Data length
 */
void cats_interleave(uint8_t* data, size_t len);

/**
 * @brief Deinterleave the supplied data as per the CATS standard
 *
 * @param data Input and output buffer
 * @param len Data length
 */
void cats_deinterleave(uint8_t* data, size_t len);

#endif // CATS_INTERLEAVER_H