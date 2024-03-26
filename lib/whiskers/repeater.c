#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/util.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

size_t cats_repeater_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_repeater_whisker_t* repeater = &(data->repeater);

    int16_t lat = lat_to_int16(repeater->latitude);
    int16_t lon = lon_to_int16(repeater->longitude);

    dest[2] = repeater->uplink;
    dest[3] = repeater->uplink >> 8;
    dest[4] = repeater->uplink >> 16;
    dest[5] = repeater->uplink >> 24;

    dest[6] = repeater->downlink;
    dest[7] = repeater->downlink >> 8;
    dest[8] = repeater->downlink >> 16;
    dest[9] = repeater->downlink >> 24;

    dest[10] = repeater->modulation;

    dest[11] = repeater->tone;
    dest[12] = repeater->tone >> 8;
    dest[13] = repeater->tone >> 16;

    dest[14] = repeater->power;
    
    dest[15] = lat;
    dest[16] = lat >> 8;

    dest[17] = lon;
    dest[18] = lon >> 8;

    strcpy(&dest[19], repeater->name);
    return strlen(repeater->name) + 17;
}

void cats_repeater_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    int16_t lat = (data[16] << 8) | data[15];
    int16_t lon = (data[18] << 8) | data[17];

    cats_repeater_whisker_t* repeater = &(dest->repeater);
    repeater->uplink = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];
    repeater->downlink = (data[9] << 24) | (data[8] << 16) | (data[7] << 8) | data[6];
    repeater->modulation = data[10];
    repeater->tone = (data[13] << 16) | (data[12] << 8) | data[11];
    repeater->power = data[14];
    repeater->latitude = int16_to_lat(lat);
    repeater->longitude = int16_to_lon(lon);
    memcpy(repeater->name, &data[19], len-17);
}

int cats_packet_get_repeater(const cats_packet_t* pkt, cats_repeater_whisker_t*** out)
{
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_REPEATER, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_repeater: packet has no repeater whiskers!");
	}
	
	(*out) = malloc(sizeof(cats_repeater_whisker_t*)*whiskers_found);
	if((*out) == NULL) {
		throw(MALLOC_FAIL);
	}
	for(int i = 0; i < whiskers_found; i++) {
		(*out)[i] = &(whiskers[i]->data.repeater);
	}
	free(whiskers);

	return whiskers_found;
}

int cats_packet_add_repeater(cats_packet_t* pkt, uint32_t up, uint32_t down, cats_modulation_t modulation, uint32_t tone, uint8_t power, double lat, double lon, const char* name)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_REPEATER) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}

	cats_repeater_whisker_t repeater;
	repeater.uplink = up;
	repeater.downlink = down;
	repeater.latitude = lat;
	repeater.longitude = lon;
	repeater.modulation = modulation;
	repeater.power = power;
	repeater.tone = tone;
	strcpy(repeater.name, name);

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_REPEATER, (cats_whisker_data_t*)&repeater, cats_whisker_base_len(WHISKER_TYPE_REPEATER) + strlen(name));
}