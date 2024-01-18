#include "cats/ldpc.h"
#include <string.h>

void cats_ldpc_encode(uint8_t* data, int len)
{
    uint8_t out[len];
    memcpy(out, data, len);

    // Split data into blocks and encode each block
    int lenRemaining = len;
    cats_ldpc_code code;
    while(lenRemaining > 0)
    {
        code = cats_ldpc_pick_code(lenRemaining);
        switch(code)
        {
            case LDPC_TC128:
                // TODO: Pad data
                //if(lenRemaining < 128)
                break;
            case LDPC_TC256:
                break;
            case LDPC_TC512:
                break;
            case LDPC_TM2048:
                break;
            case LDPC_TM8192:
                break;
        }
        lenRemaining -= code;
    }

}

void cats_ldpc_decode(uint8_t* data, int len)
{

}

cats_ldpc_code cats_ldpc_pick_code(int len)
{
    if(8192 <= len)
        return LDPC_TM8192;
    else if(2048 <= len)
        return LDPC_TM2048;
    else if(512 <= len)
        return LDPC_TC512;
    else if(256 <= len)
        return LDPC_TC256;
    else
        return LDPC_TC128;
}