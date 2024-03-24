#include "cats/whisker.h"
#include "cats/error.h"
#include "cats/util.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef size_t (*whisker_encode_func_t)(const cats_whisker_data_t* data, uint8_t* dest);
typedef void (*whisker_decode_func_t)(const uint8_t* data, size_t len, cats_whisker_data_t* dest);

// Encoders
extern size_t cats_ident_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_gps_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_route_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_dest_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_simplex_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_repeater_encode(const cats_whisker_data_t* data, uint8_t* dest);
extern size_t cats_nodeinfo_encode(const cats_whisker_data_t* data, uint8_t* dest);

// Decoders
extern void cats_ident_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_gps_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_route_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_dest_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_simplex_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_repeater_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);
extern void cats_nodeinfo_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest);

static uint8_t base_lengths[] = {
    3,  // IDENTIFICATION
    5,  // TIMESTAMP
    14, // GPS
    0,  // COMMENT
    1,  // ROUTE
    2,  // DESTINATION
    0,  // ARBITRARY
    6,  // SIMPLEX
    17, // REPEATER
    3,  // NODEINFO
};

whisker_encode_func_t encoders[] = {
    &cats_ident_encode,
    NULL, // TIMESTAMP
    &cats_gps_encode,
    NULL, // COMMENT
    &cats_route_encode,
    &cats_dest_encode,
    NULL, // ARBITRARY
    &cats_simplex_encode,
    &cats_repeater_encode,
    &cats_nodeinfo_encode
};

whisker_decode_func_t decoders[] = {
    &cats_ident_decode,
    NULL, // TIMESTAMP
    &cats_gps_decode,
    NULL, // COMMENT
    &cats_route_decode,
    &cats_dest_decode,
    NULL, // ARBITRARY
    &cats_simplex_decode,
    &cats_repeater_decode,
    &cats_nodeinfo_decode
};

int cats_whisker_encode(const cats_whisker_t* whisker, uint8_t* out)
{
    out[0] = whisker->type;
    out[1] = whisker->len;

    const cats_whisker_data_t* data = &whisker->data;
    if(whisker->type == WHISKER_TYPE_ARBITRARY
    || whisker->type == WHISKER_TYPE_COMMENT
    || whisker->type == WHISKER_TYPE_TIMESTAMP) {
        memcpy(&out[2], data->raw, whisker->len);
    }
    else if(whisker->type < CATS_NUM_WHISKER_TYPES) { // If whisker type is supported
        encoders[whisker->type](data, out);
    }
    else {
        throw(UNSUPPORTED_WHISKER);
    }

    return CATS_SUCCESS;
}

int cats_whisker_decode(const uint8_t* data, cats_whisker_t* out)
{
    out->type = data[0];
	out->len = data[1];

    cats_whisker_data_t* whisker_data = &(out->data);
    memset(whisker_data, 0x00, sizeof(cats_whisker_data_t));

    if(out->type == WHISKER_TYPE_ARBITRARY
    || out->type == WHISKER_TYPE_COMMENT
    || out->type == WHISKER_TYPE_TIMESTAMP) {
        memcpy(whisker_data->raw, &data[2], out->len);
    }
    else if(out->type < CATS_NUM_WHISKER_TYPES) { // If whisker type is supported
        decoders[out->type](data, out->len, whisker_data);
    }
    else {
        throw(UNSUPPORTED_WHISKER);
    }

    return CATS_SUCCESS;
}

int cats_whisker_base_len(const cats_whisker_type_t type)
{
    if(type < 0 || type >= CATS_NUM_WHISKER_TYPES)
        throw(UNSUPPORTED_WHISKER);

    return base_lengths[type];
}