#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

size_t cats_dest_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_destination_whisker_t* destination = &(data->destination);
    int ptr = 2;

    dest[ptr++] = destination->ack;
    strcpy(&dest[ptr], destination->callsign);
    ptr += strlen(destination->callsign);
    dest[ptr++] = destination->ssid;

    return ptr - 2;
}

void cats_dest_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    cats_destination_whisker_t* destination = &(dest->destination);

    destination->ack = data[2];
    memcpy(destination->callsign, &data[3], len - 2);
    destination->ssid = data[len+1];
}

int cats_packet_get_destination(const cats_packet_t* pkt, cats_destination_whisker_t*** out)
{
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_DESTINATION, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_destination: packet has no destination whiskers!");
	}
	
	(*out) = malloc(sizeof(cats_destination_whisker_t*)*whiskers_found);
	if((*out) == NULL) {
		throw(MALLOC_FAIL);
	}
	for(int i = 0; i < whiskers_found; i++) {
		(*out)[i] = &(whiskers[i]->data.destination);
	}
	free(whiskers);

	return whiskers_found;
}

int cats_packet_add_destination(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint8_t ack)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_DESTINATION) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}

	cats_destination_whisker_t dest;
	dest.ack = ack;
	dest.ssid = ssid;
	strcpy(dest.callsign, callsign);

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_DESTINATION, (cats_whisker_data_t*)&dest, cats_whisker_base_len(WHISKER_TYPE_DESTINATION) + strlen(callsign));
}