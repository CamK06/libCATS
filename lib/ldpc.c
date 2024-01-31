#include "cats/ldpc.h"
#include "cats/error.h"

#include <labrador_ldpc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "cats/util.h"
#include "cats/ldpc_matrices.h"

int cats_ldpc_pick_code(int len)
{
    if(512 <= len)
        return LABRADOR_LDPC_CODE_TM8192;
    else if(128 <= len)
        return LABRADOR_LDPC_CODE_TM2048;
    else if(32 <= len)
        return LABRADOR_LDPC_CODE_TC512;
    else if(16 <= len)
        return LABRADOR_LDPC_CODE_TC256;
    else
        return LABRADOR_LDPC_CODE_TC128;
}

// Temporary: Will replace pick_code once decoding is implemented
cats_ldpc_code_t* cats_ldpc_pick_code_2(int len)
{
    if(512 <= len)
        return &tm8192;
    else if(128 <= len)
        return &tm2048;
    else if(32 <= len)
        return &tc512;
    else if(16 <= len)
        return &tc256;
    else
        return &tc128;
}

void cats_ldpc_encode_chunk(uint8_t* data, cats_ldpc_code_t* code)
{
    // Code parameters
    int k = code->k;
    int r = code->n - code->k;
    int b = code->circulantSize;
    const uint64_t* gc = code->matrix;
    int rowLen = r/64;

    uint8_t parity[(code->k)];
    memset(parity, 0x00, (code->k)/8);

    // Thank you ChatGPT ...for making me feel stupid...
    // (my own port of this code that I couldn't get working was wrong by merely a few characters)
    for (int offset = 0; offset < b; offset++) {
        for (int crow = 0; crow < k/b; crow++) {
            int bit = crow*b + offset;
            if(GET_BIT(data[bit/8], bit%8)) {
                for (int idx = 0; idx < rowLen; idx++) {
                    uint64_t circ = gc[(crow*rowLen)+idx];
                    parity[idx*8 + 7] ^= (uint8_t)(circ >>  0);
                    parity[idx*8 + 6] ^= (uint8_t)(circ >>  8);
                    parity[idx*8 + 5] ^= (uint8_t)(circ >> 16);
                    parity[idx*8 + 4] ^= (uint8_t)(circ >> 24);
                    parity[idx*8 + 3] ^= (uint8_t)(circ >> 32);
                    parity[idx*8 + 2] ^= (uint8_t)(circ >> 40);
                    parity[idx*8 + 1] ^= (uint8_t)(circ >> 48);
                    parity[idx*8 + 0] ^= (uint8_t)(circ >> 56);
                }
            }
        }

        for (int block = 0; block < r/b; block++) {
            uint8_t* parityblock = &parity[block*b/8];
            uint8_t carry = parityblock[0] >> 7;
            for (int k = (b/8)-1; k >= 0; k--) {
                uint8_t c = parityblock[k] >> 7;
                parityblock[k] = (parityblock[k] << 1) | carry;
                carry = c;
            }
        }
    }

    memcpy(data+(code->k/8), parity, code->k/8);
}

uint16_t cats_ldpc_encode(uint8_t** data, uint16_t len)
{
    uint8_t out[len*2+8]; // 8 extra bytes to accomodate padded blocks

    // Split data into blocks and encode each block
    int i = 0;
    while(len-i > 0) {
        cats_ldpc_code_t* code = cats_ldpc_pick_code_2(len-i);
        int k = code->k;
        
        uint8_t chunk[code->n/8];
        memset(chunk, 0xAA, k/8);
        memcpy(chunk, (*data)+i, (len-i < k/8) ? (len-i) : (k/8));

        cats_ldpc_encode_chunk(chunk, code);
        memcpy(out+i, chunk, (len-i < k/8) ? (len-i) : (k/8));  // Data
        memcpy(out+len+i, chunk+(k/8), k/8); // Parity

        i += k/8;
    }

    // Reallocate data buffer to new length and copy 'out' to it
    int newLen = (len*2) + 2 + (i-len); // (Data+Parity) + (Length) + (Padded parity)
    
    uint8_t* tmp = realloc(*data, newLen);
    if(tmp == NULL)
        throw(MALLOC_FAIL);
    *data = tmp; 

    memcpy(*data, out, newLen);
    memcpy((*data)+newLen-2, &len, sizeof(uint16_t)); // Set the last 2 bytes to the length
    return newLen;
}

uint16_t cats_ldpc_decode(uint8_t** buf, uint16_t bufLen)
{
    if(bufLen < 2)
        throw(LDPC_DECODE_FAIL);

    uint16_t len;
    memcpy(&len, (*buf)+bufLen-2, sizeof(uint16_t));
    if(len >= bufLen)
        throw(LDPC_DECODE_FAIL);

    uint8_t out[len];

    int i = 0;
    while(len-i > 0) {
        int code = cats_ldpc_pick_code(len-i);
        int k = labrador_ldpc_code_k(code);

        uint8_t chunk[labrador_ldpc_code_n(code)/8];
        memset(chunk, 0xAA, k/8);
        memcpy(chunk, (*buf)+i, (len-i < k/8) ? (len-i) : (k/8)); // Data
        memcpy(chunk+(k/8), (*buf)+len+i, k/8); // Parity

        uint8_t decOut[labrador_ldpc_output_len(code)];
        uint8_t working[LABRADOR_LDPC_BF_WORKING_LEN_TM8192];
        labrador_ldpc_decode_bf(code, chunk, decOut, working, 16, NULL);
        memcpy(out+i, decOut, (len-i < k/8) ? (len-i) : (k/8));
        i += k/8;
    }

    uint8_t* tmp = realloc(*buf, len);
    if(tmp == NULL)
        throw(MALLOC_FAIL);
    *buf = tmp;

    memset(*buf, 0x00, len);
    memcpy(*buf, out, len);
    return len;
}