#include "cats/whitener.h"

#include <stdint.h>
#include <stddef.h>

#define START_STATE 0xE9CF

uint8_t lfsr_byte(uint16_t* state);
void lfsr(uint16_t* state);

void cats_whiten(uint8_t* data, size_t len)
{
	uint16_t state = START_STATE;
	for(int i = 0; i < len; i++) {
		uint8_t byte = lfsr_byte(&state);
		data[i] ^= byte;
	}
}

uint8_t lfsr_byte(uint16_t* state)
{
	uint8_t out = 0;
	for(int i = 7; i >= 0; i--) {
		out |= (*state & 1) << i;
		lfsr(state);
	}
	return out;
}

void lfsr(uint16_t* state)
{
	uint8_t lsb = *state & 1u;
	*state >>= 1;
	if(lsb) {
		*state ^= 0xB400u;
	}
}
 