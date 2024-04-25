#ifndef CATS_LDPC_H
#define CATS_LDPC_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief LDPC encode a buffer of data
 * 
 * @param data Input and output buffer
 * @param len Input length
 * @return New data length
 * 
 * @note The `data` buffer must have enough room to accomodate the encoded data (about 2x the input length)
 */
size_t cats_ldpc_encode(uint8_t* data, size_t len);

/**
 * @brief LDPC decode a buffer of data
 * 
 * @param buf Input and output buffer
 * @param buf_len Input length
 * @return New data length or `CATS_FAIL` if decode failed
 * @note Throws LDPC_DECODE_FAIL on failure
 */
size_t cats_ldpc_decode(uint8_t* buf, size_t buf_len);

#endif // CATS_LDPC_H