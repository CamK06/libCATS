#include "cats/interleaver.h"
#include "cats/util.h"

#include <string.h>

void cats_interleave(uint8_t* data, int len)
{
	uint8_t buf[len];
	int bitIndex = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len*8; j += 32) {
			if(i+j >= len*8)
				continue;
			
			if(GET_BIT(data[(i+j)/8], (i+j)%8))
				SET_BIT(buf[bitIndex/8], bitIndex%8);
			else
				CLEAR_BIT(buf[bitIndex/8], bitIndex%8);

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

			if(GET_BIT(data[bitIndex/8], bitIndex%8))
				SET_BIT(buf[(i+j)/8], (i+j)%8);
			else
				CLEAR_BIT(buf[(i+j)/8], (i+j)%8);

			bitIndex++;	
		}
	}
	memcpy(data, buf, len);
}
