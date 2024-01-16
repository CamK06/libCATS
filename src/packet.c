#include "cats/packet.h"
#include <string.h>

int cats_packet_encode(cats_whisker_t* whiskers, int whiskerCount, uint8_t* dataOut)
{
	// Preamble + Syncword
	memset(dataOut, 0x55, 4);
	dataOut[4] = 0xAB;
	dataOut[5] = 0xCD;
	dataOut[6] = 0xEF;
	dataOut[7] = 0x12;
	
	// Packet length
	dataOut[8] = 0x00;
	dataOut[9] = 0x00;
}

int cats_packet_decode(const uint8_t* data, int len, cats_whisker_t* whiskersOut)
{	
	// Header
	if(len < 10)
		return -1;
	uint16_t pktLen;
	memcpy(&pktLen, data+8, 2);

	// LDPC Decode
	uint8_t buf[pktLen];
	
}
