#ifndef CATS_LDPC_MATRICES_H
#define CATS_LDPC_MATRICES_H

#include <stdint.h>
#include <stddef.h>

// https://github.com/adamgreig/labrador-ldpc/blob/master/src/codes/compact_generators.rs

typedef struct cats_ldpc_code {
    // Code length (data+parity)
    int n;
    // Data length in bits
    int k;
    int puncturedBits;
    int bfWorkingLen;
    size_t circulantSize;
    size_t matrixLen;
    const uint64_t* matrix;
} cats_ldpc_code_t;

extern cats_ldpc_code_t tc128;
extern cats_ldpc_code_t tc256;
extern cats_ldpc_code_t tc512;
extern cats_ldpc_code_t tm2048;
extern cats_ldpc_code_t tm8192;

extern const uint64_t tc128_matrix[];
extern const uint64_t tc256_matrix[];
extern const uint64_t tc512_matrix[];
extern const uint64_t tm2048_matrix[];
extern const uint64_t tm8192_matrix[];

#endif
