/*
libCATS LDPC Test

Ensures the LDPC encoder returns packets of the appropriate length, the decoder works properly,
and that padding (as is specific to CATS for variable length packets) chunks of a packet works.
*/

#include "cats/ldpc.h"
#include "util.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

void test_len()
{
    uint8_t* buf = calloc(8191, 1);
    int len = 0;
    for(int i = 0; i < 41; i++) {
        len = i*56;
        strcpy(buf+len, "Example packet data  wueirpqwerwrywqoeiruy29346129384761");
    }
    strcpy(buf+len+56, "Example packet data  wueirpqwerwrywqoeiru346129384761");
    len += 56+53;
    
    assert(len == 2349);
    len = cats_ldpc_encode(buf, len);
    assert(len == 4703);

    free(buf);
}

void test_encode_decode_short()
{
    uint8_t* buf = calloc(32, 1);
    uint8_t* orig = calloc(32, 1);
    strcpy(orig, "Hello world!");
    strcpy(buf, orig);

    int len = cats_ldpc_encode(buf, strlen(buf)+1);
    assert(memcmp(buf, orig, len) != 0);
    len = cats_ldpc_decode(buf, len);
    assert(memcmp(buf, orig, len) == 0);

    free(buf);
    free(orig);
}

void test_encode_decode()
{
    uint8_t* buf = calloc(8191, 1);
    uint8_t* orig = calloc(8191, 1);
    for(int i = 0; i < 50; i++)
        strcpy(orig+i*58, "This is a test packet. jsalksjd093809324JASLD:LKD*#$)(*#@)");
    strcpy(buf, orig);

    int len = cats_ldpc_encode(buf, strlen(buf)+1);
    assert(memcmp(buf, orig, len) != 0);
    len = cats_ldpc_decode(buf, len);
    assert(memcmp(buf, orig, len) == 0);

    free(buf);
    free(orig);
}

void test_encode_decode_bit_flips()
{
    uint8_t* buf = calloc(8191, 1);
    uint8_t* orig = calloc(8191, 1);
    for(int i = 0; i < 50; i++)
        strcpy(orig+i*55, "jsalksjd093809324JASLD:LKD*#$)(*#@) Another test packet");
    strcpy(buf, orig);

    int len = cats_ldpc_encode(buf, strlen(buf)+1);
    assert(memcmp(buf, orig, len) != 0);

    buf[234] ^= 0x55;
    buf[0] ^= 0xAA;
    buf[999] ^= 0x43;

    len = cats_ldpc_decode(buf, len);
    assert(memcmp(buf, orig, len) == 0);

    free(buf);
    free(orig);
}

int main()
{
    test_len();
    test_encode_decode_short();
    test_encode_decode();
    test_encode_decode_bit_flips();
    return 0;
}