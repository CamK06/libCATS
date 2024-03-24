#include "cats/whisker.h"

#include <stddef.h>
#include <string.h>

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