#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

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

int cats_packet_get_simplex(const cats_packet_t* pkt, cats_simplex_whisker_t*** out)
{
	assert(pkt != NULL);
	assert(out != NULL);
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_SIMPLEX, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_simplex: packet has no simplex whiskers!");
	}
	
	(*out) = malloc(sizeof(cats_simplex_whisker_t*) * whiskers_found);
	if((*out) == NULL) {
		throw(MALLOC_FAIL);
	}
	for(int i = 0; i < whiskers_found; i++) {
		(*out)[i] = &(whiskers[i]->data.simplex);
	}
	free(whiskers);
	
	return whiskers_found;
}

int cats_packet_add_simplex(cats_packet_t* pkt, uint32_t frequency, cats_modulation_t modulation, uint8_t power)
{
	assert(pkt != NULL);
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_SIMPLEX) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}

	cats_simplex_whisker_t simplex;
	simplex.frequency = frequency;
	simplex.modulation = modulation;
	simplex.power = power;

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_SIMPLEX, (cats_whisker_data_t*)&simplex, cats_whisker_base_len(WHISKER_TYPE_SIMPLEX));
}