#include "cats/whisker.h"

#include <stddef.h>

size_t cats_simplex_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_simplex_whisker_t* simplex = &(data->simplex);

    dest[2] = simplex->frequency;
    dest[3] = simplex->frequency >> 8;
    dest[4] = simplex->frequency >> 16;
    dest[5] = simplex->frequency >> 24;
    
    dest[6] = simplex->modulation;
    dest[7] = simplex->power;

    return 6;
}

void cats_simplex_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    cats_simplex_whisker_t* simplex = &(dest->simplex);

    simplex->frequency = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];
    simplex->modulation = data[6];
    simplex->power = data[7];
}