#ifndef CATS_PACKET_H
#define CATS_PACKET_H

#include "whisker.h"
#include <stdint.h>

// Encode a CATS packet from source 
// Returns the number of output bytes. Returns -1 if an error occurred.
int cats_packet_encode(cats_whisker_t* whiskers, int whiskerCount, uint8_t* dataOut);
// Decode a CATS packet into its individual whiskers
// Returns the number of whiskers. Returns -1 if the packet is invalid.
int cats_packet_decode(const uint8_t* data, int len, cats_whisker_t* whiskersOut);
// Calculate the CRC16 of a buffer
uint16_t cats_crc16(uint8_t* data, int len);

#endif
