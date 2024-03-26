#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/util.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

size_t cats_gps_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_gps_whisker_t* gps = &(data->gps);
    int32_t lat = lat_to_int32(gps->latitude);
    int32_t lon = lon_to_int32(gps->longitude);
    uint16_t alt = float32_to_float16(gps->altitude);
    uint16_t vel = float32_to_float16(gps->speed);

    dest[2] = lat;
    dest[3] = lat >> 8;
    dest[4] = lat >> 16;
    dest[5] = lat >> 24;

    dest[6] = lon;
    dest[7] = lon >> 8;
    dest[8] = lon >> 16;
    dest[9] = lon >> 24;

    dest[10] = alt;
    dest[11] = alt >> 8;

    dest[12] = gps->max_error;
    dest[13] = gps->heading;
    
    dest[14] = vel;
    dest[15] = vel >> 8;

    return 14; // GPS whisker length
}

void cats_gps_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    int32_t lat = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];
    int32_t lon = (data[9] << 24) | (data[8] << 16) | (data[7] << 8) | data[6];
    uint16_t alt = (data[11] << 8) | data[10];
    uint16_t vel = (data[15] << 8) | data[14];

    cats_gps_whisker_t* gps = &(dest->gps);
    gps->max_error = data[12];
    gps->heading = data[13];
    gps->latitude = int32_to_lat(lat);
    gps->longitude = int32_to_lon(lon);
    gps->altitude = float16_to_float32(alt);
    gps->speed = float16_to_float32(vel);
}

int cats_packet_get_gps(const cats_packet_t* pkt, cats_gps_whisker_t** out)
{
    assert(pkt != NULL);
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_GPS, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_gps: packet has no GPS whiskers!");
	}
    assert(out != NULL);
	cats_whisker_t* whisker = *whiskers;
	free(whiskers);

	*out = &whisker->data.gps;

	return CATS_SUCCESS;
}

int cats_packet_add_gps(cats_packet_t* pkt, double lat, double lon, float alt, uint8_t error, uint8_t heading, float speed)
{
    assert(pkt != NULL);
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_GPS) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}
	if(cats_packet_get_gps(pkt, NULL) != CATS_FAIL) {
		throw(MAX_WHISKERS_OF_TYPE_EXCEEDED);
	}

	cats_gps_whisker_t gps;
	gps.altitude = alt;
	gps.heading = heading;
	gps.speed = speed;
	gps.max_error = error;
	gps.latitude = lat;
	gps.longitude = lon;

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_GPS, (cats_whisker_data_t*)&gps, cats_whisker_base_len(WHISKER_TYPE_GPS));
}