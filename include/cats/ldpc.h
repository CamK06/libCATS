#ifndef CATS_LDPC_H
#define CATS_LDPC_H

#include <stdint.h>

// LDPC encode a buffer of data
// Returns: New buffer length
uint16_t cats_ldpc_encode(uint8_t** data, uint16_t len);
// LDPC decode a buffer of data
// Returns: New buffer length
uint16_t cats_ldpc_decode(uint8_t** buf, uint16_t bufLen);

#endif