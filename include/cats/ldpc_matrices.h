#ifndef CATS_LDPC_MATRICES_H
#define CATS_LDPC_MATRICES_H

#include <stdint.h>
#include <stddef.h>

// https://github.com/adamgreig/labrador-ldpc/blob/master/src/codes/compact_generators.rs

typedef struct {
    // Code length in bits (data+parity)
    int code_length_bits;
    // Data length in bits
    int data_length_bits;
    int punctured_bits;
    int bf_working_len;
    size_t circulant_size;
    size_t matrix_len;
    const uint64_t* matrix;
} cats_ldpc_code;

extern cats_ldpc_code tc128;
extern cats_ldpc_code tc256;
extern cats_ldpc_code tc512;
extern cats_ldpc_code tm2048;
extern cats_ldpc_code tm8192;

extern const uint64_t tc128_matrix[];
extern const uint64_t tc256_matrix[];
extern const uint64_t tc512_matrix[];
extern const uint64_t tm2048_matrix[];
extern const uint64_t tm8192_matrix[];

#endif
