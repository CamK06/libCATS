#include "cats/ldpc.h"

#include <labrador_ldpc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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

uint16_t cats_ldpc_encode(uint8_t** data, uint16_t len)
{
    uint8_t* out = malloc(len*2+8); // 8 extra bytes to accomodate padded blocks

    // Split data into blocks and encode each block
    int i = 0;
    while(len-i > 0) {
        int code = cats_ldpc_pick_code(len-i);
        int k = labrador_ldpc_code_k(code);
        
        uint8_t chunk[labrador_ldpc_code_n(code)/8];
        memset(chunk, 0xAA, k/8);
        memset(chunk+(k/8), 0x00, k/8);
        memcpy(chunk, (*data)+i, (len-i < k/8) ? (len-i) : (k/8));

        labrador_ldpc_encode(code, chunk);
        memcpy(out+i, chunk, (len-i < k/8) ? (len-i) : (k/8));  // Data
        memcpy(out+len+i, chunk+(k/8), k/8); // Parity

        i += k/8;
    }

    // Reallocate data buffer to new length and copy 'out' to it
    int newLen = len*2+2+i-len; // Data+Parity+Padded Parity+Length
    uint8_t* tmp = realloc(*data, newLen);
    assert(tmp != NULL); // TODO: Handle realloc failure
    *data = tmp; 
    memcpy(*data, out, newLen);
    memcpy((*data)+newLen-2, &len, sizeof(uint16_t));
    free(out);
    
    return newLen;
}

uint16_t cats_ldpc_decode(uint8_t** buf, uint16_t bufLen)
{
    if(bufLen < 2) // TODO: Throw a proper error here
        return -1;

    uint16_t len;
    memcpy(&len, (*buf)+bufLen-2, sizeof(uint16_t));
    if(len >= bufLen) // Error
        return -1;

    uint8_t* out = malloc(len);

    int i = 0;
    while(len-i > 0) {
        int code = cats_ldpc_pick_code(len-i);
        int k = labrador_ldpc_code_k(code);

        uint8_t chunk[labrador_ldpc_code_n(code)/8];
        memset(chunk, 0xAA, k/8);
        memcpy(chunk, (*buf)+i, (len-i < k/8) ? (len-i) : (k/8));
        memcpy(chunk+(k/8), (*buf)+len+i, k/8);

        uint8_t decOut[labrador_ldpc_output_len(code)];
        uint8_t working[LABRADOR_LDPC_BF_WORKING_LEN_TM8192];
        labrador_ldpc_decode_bf(code, chunk, decOut, working, 16, NULL);
        memcpy(out+i, decOut, (len-i < k/8) ? (len-i) : (k/8));
        i += k/8;
    }

    uint8_t* tmp = realloc(*buf, len);
    assert(tmp != NULL); // TODO: Handle realloc failure
    *buf = tmp;
    memset(*buf, 0x00, len);
    memcpy(*buf, out, len);
    free(out);

    return len;
}