#include "cats/whisker.h"
#include "cats/util.h"

#include <stddef.h>
#include <stdint.h>

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

    dest[12] = gps->maxError;
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
    gps->maxError = data[12];
    gps->heading = data[13];
    gps->latitude = int32_to_lat(lat);
    gps->longitude = int32_to_lon(lon);
    gps->altitude = float16_to_float32(alt);
    gps->speed = float16_to_float32(vel);
}