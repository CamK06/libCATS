#ifndef CATS_LDPC_H
#define CATS_LDPC_H

#include <stdint.h>

typedef enum cats_ldpc_code {
    LDPC_TC128  = 128,
    LDPC_TC256  = 256,
    LDPC_TC512  = 512,
    LDPC_TM2048 = 2048,
    LDPC_TM8192 = 8192
} cats_ldpc_code;

void cats_ldpc_encode(uint8_t* data, int len);
void cats_ldpc_decode(uint8_t* data, int len);
cats_ldpc_code cats_ldpc_pick_code(int len);


#endif