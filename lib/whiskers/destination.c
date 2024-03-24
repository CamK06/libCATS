#include "cats/whisker.h"

#include <stddef.h>
#include <string.h>

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