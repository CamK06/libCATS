#include "cats/whisker.h"
#include "cats/util.h"

#include <stddef.h>
#include <string.h>

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