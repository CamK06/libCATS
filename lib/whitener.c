#include "cats/whitener.h"

void cats_whiten(uint8_t* data, int len)
{
	uint16_t state = LFSR_START_STATE;
	for(int i = 0; i < len; i++) {
		uint8_t byte = cats_lfsr_byte(&state);
		data[i] ^= byte;
	}
}

uint8_t cats_lfsr_byte(uint16_t* state)
{
	uint8_t out = 0;
	for(int i = 0; i < 8; i++) {
		out |= (*state & 1u) << 7-i;
		cats_lfsr(state);
	}
	return out;
}

uint8_t cats_lfsr(uint16_t* state)
{
	uint8_t out = 0;
	uint8_t lsb = *state & 1u;
	*state >>= 1;
	if(lsb)
		*state ^= 0xB400u;
	return out;
}
