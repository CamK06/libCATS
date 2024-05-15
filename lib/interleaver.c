#include "cats/interleaver.h"
#include "cats/util.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void cats_interleave(uint8_t* data, size_t len)
{
	uint8_t buf[len];
	int bit_index = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len * 8; j += 32) {
			if(i+j >= len * 8) {
				continue;
			}
			
			if(GET_BIT(data[(i+j) / 8], (i+j) % 8)) {
				SET_BIT(buf[bit_index / 8], bit_index % 8);
			}
			else {
				CLEAR_BIT(buf[bit_index / 8], bit_index % 8);
			}

			bit_index++;	
		}	
	}
	memcpy(data, buf, len);
}

void cats_deinterleave(uint8_t* data, size_t len)
{
	uint8_t buf[len];
	int bit_index = 0;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < len * 8; j+= 32) {
			if(i+j >= len * 8) {
				continue;
			}

			if(GET_BIT(data[bit_index / 8], bit_index % 8)) {
				SET_BIT(buf[(i+j) / 8], (i+j) % 8);
			}
			else {
				CLEAR_BIT(buf[(i+j) / 8], (i+j) % 8);
			}

			bit_index++;	
		}
	}
	memcpy(data, buf, len);
}
