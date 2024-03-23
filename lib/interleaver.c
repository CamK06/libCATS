#include "cats/interleaver.h"
#include "cats/util.h"

#include <stddef.h>
#include <stdint.h>

void cats_interleave(uint8_t* dest, const uint8_t* src, size_t len)
{
	int bitIndex = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len*8; j += 32) {
			if(i+j >= len*8)
				continue;
			
			if(GET_BIT(src[(i+j)/8], (i+j)%8))
				SET_BIT(dest[bitIndex/8], bitIndex%8);
			else
				CLEAR_BIT(dest[bitIndex/8], bitIndex%8);

			bitIndex++;	
		}	
	}
}

void cats_deinterleave(uint8_t* dest, const uint8_t* src, size_t len)
{
	int bitIndex = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len*8; j+= 32) {
			if(i+j >= len*8)
				continue;

			if(GET_BIT(src[bitIndex/8], bitIndex%8))
				SET_BIT(dest[(i+j)/8], (i+j)%8);
			else
				CLEAR_BIT(dest[(i+j)/8], (i+j)%8);

			bitIndex++;	
		}
	}
}
