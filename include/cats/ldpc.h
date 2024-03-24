#ifndef CATS_LDPC_H
#define CATS_LDPC_H

#include <stdint.h>
#include <stddef.h>

// LDPC encode a buffer of data
// Returns: New buffer length
size_t cats_ldpc_encode(uint8_t* data, size_t len);
// LDPC decode a buffer of data
// Returns: New buffer length
size_t cats_ldpc_decode(uint8_t* buf, size_t buf_len);

#endif