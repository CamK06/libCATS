#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/util.h"

#include <stdlib.h>
#include <assert.h>

static void uint64_to_5byte(uint64_t value, uint8_t* result) {
    result[0] = (value >> 32) & 0xFF;
    result[1] = (value >> 24) & 0xFF;
    result[2] = (value >> 16) & 0xFF;
    result[3] = (value >> 8) & 0xFF;
    result[4] = value & 0xFF;
}

uint64_t cats_packet_get_timestamp(const cats_packet_t* pkt)
{
	assert(pkt != NULL);
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_TIMESTAMP, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_timestamp: packet has no timestamp whisker!");
	}
	
    uint64_t timestamp = 0;
    uint8_t* data = (*whiskers)->data.raw;
	timestamp = ((uint64_t)data[4] << 32) | (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

	free(whiskers);
	return timestamp;
}

int cats_packet_add_timestamp(cats_packet_t* pkt, uint64_t timestamp)
{
	assert(pkt != NULL);
	if(pkt->len + 7 > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}
	if(cats_packet_get_timestamp(pkt) != CATS_FAIL) {
		throw(MAX_WHISKERS_OF_TYPE_EXCEEDED);
	}

	cats_whisker_t whisker;
    whisker.type = WHISKER_TYPE_TIMESTAMP;
    whisker.len = 5;
    whisker.data.raw[0] = timestamp;
    whisker.data.raw[1] = (timestamp >> 8);
    whisker.data.raw[2] = (timestamp >> 16);
    whisker.data.raw[3] = (timestamp >> 24);
    whisker.data.raw[4] = (timestamp >> 32);

	return cats_packet_add_whisker(pkt, &whisker);
}