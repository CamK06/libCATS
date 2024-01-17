/*
libCATS Interleaver Test

Attempts to interleave, then de-interleave data and compares
the output with known-good data from the original Rust implementation
of CATS.
*/

#include <string.h>
#include "cats/interleaver.h"

int main()
{
    uint8_t buf[] = {0x84, 0x73, 0x12, 0xA3, 0xFF, 0x00, 0xC2, 0x1B, 0x77};
	uint8_t orig[] = {0x84, 0x73, 0x12, 0xA3, 0xFF, 0x00, 0xC2, 0x1B, 0x77};
	uint8_t expect[] = {0xCD, 0xB5, 0xDB, 0x2A, 0x0A, 0x52, 0x0C, 0x89, 0x4F};
	    
    cats_interleave(buf, 9);
	if(memcmp(buf, expect, 9) != 0)
		return -1;

	cats_deinterleave(buf, 9);
	if(memcmp(buf, orig, 9) != 0)
		return -1;

    return 0;
}