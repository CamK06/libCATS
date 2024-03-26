#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

size_t cats_ident_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_ident_whisker_t* ident = &(data->identification);
    int ptr = 2;
    
    dest[ptr++] = ident->icon;
    dest[ptr++] = ident->icon >> 8;
    strcpy(&dest[ptr], ident->callsign);
    ptr += strlen(ident->callsign);
    dest[ptr++] = ident->ssid;

    return ptr - 2;
}

void cats_ident_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    cats_ident_whisker_t* ident = &(dest->identification);
    memcpy(ident->callsign, &data[4], len - 3);
    ident->icon = (data[3] << 8) | data[2];
    ident->ssid = data[len+1];
}

int cats_packet_get_identification(const cats_packet_t* pkt, cats_ident_whisker_t** out)
{
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_IDENTIFICATION, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_identification: packet has no identification whisker!");
	}
	cats_whisker_t* whisker = *whiskers;
	*out = &whisker->data.identification;

	free(whiskers);
	return CATS_SUCCESS;
}

int cats_packet_add_identification(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint16_t icon)
{
	if(callsign == NULL || strlen(callsign) <= 0 
	|| strlen(callsign) > 252) {
		throw(INVALID_OR_NO_CALLSIGN);
	}
	if(pkt->len + 5 + strlen(callsign) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}
	if(cats_packet_get_identification(pkt, NULL) != CATS_FAIL) {
		throw(MAX_WHISKERS_OF_TYPE_EXCEEDED);
	}

	cats_ident_whisker_t identification;
	identification.ssid = ssid;
	identification.icon = icon;
	strcpy(identification.callsign, callsign);

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_IDENTIFICATION, (cats_whisker_data_t*)&identification, 3 + strlen(callsign));
}