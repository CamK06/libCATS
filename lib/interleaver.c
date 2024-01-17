#include "cats/interleaver.h"
#include <string.h>

void cats_interleave(uint8_t* data, int len)
{
	uint8_t buf[len];

	int bitIndex = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len*8; j += 32) {
			if(i+j >= len*8)
				continue;
			
			if((data[(i+j)/8] & (1 << 7-(i+j)%8)) != 0)
				buf[bitIndex/8] |= 1 << 7-bitIndex%8;
			else
				buf[bitIndex/8] &= ~(1 << 7-bitIndex%8);

			bitIndex++;	
		}	
	}

	memcpy(data, buf, len);
}

void cats_deinterleave(uint8_t* data, int len)
{
	uint8_t buf[len];
	
	int bitIndex = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len*8; j+= 32) {
			if(i+j >= len*8)
				continue;

			if((data[bitIndex/8] & (1 << 7-bitIndex%8)) != 0)
				buf[(i+j)/8] |= 1 << 7-(i+j)%8;
			else
				buf[(i+j)/8] &= ~(1 << 7-(i+j)%8);

			bitIndex++;	
		}
	}

	memcpy(data, buf, len);
}
