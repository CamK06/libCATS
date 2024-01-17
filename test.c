/* 
 * libCATS Test Program
 * this program tests the full functionality of libCATS to ensure compliance
 * with the CATS standard.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cats/interleaver.h"

int main()
{
	printf("libCATS Test Program\n");
	printf("Interleaver...");
	uint8_t buf[] = {0x84, 0x73, 0x12, 0xA3, 0xFF, 0x00, 0xC2, 0x1B, 0x77};
	uint8_t orig[] = {0x84, 0x73, 0x12, 0xA3, 0xFF, 0x00, 0xC2, 0x1B, 0x77};
	uint8_t expect[] = {0xCD, 0xB5, 0xDB, 0x2A, 0x0A, 0x52, 0x0C, 0x89, 0x4F};
	cats_interleave(buf, 9);
	if(memcmp(buf, expect, 9) == 0)
		printf("PASS\n");
	else
		printf("FAIL\n");

	printf("Deinterleaver...");
	cats_deinterleave(buf, 9);
	if(memcmp(buf, orig, 9) == 0)
		printf("PASS\n");
	else
		printf("FAIL\n");

	return 0;
}
