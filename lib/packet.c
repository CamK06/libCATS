#include "cats/interleaver.h"
#include "cats/whitener.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/ldpc.h"
#include "cats/util.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

uint16_t cats_crc16(uint8_t* data, int len);

int cats_packet_prepare(cats_packet_t** pkt)
{
	*pkt = malloc(sizeof(cats_packet_t));
	if((*pkt) == NULL) {
		throw(MALLOC_FAIL);
	}

	(*pkt)->len = 0;
	(*pkt)->numWhiskers = 0;
	(*pkt)->whiskers = NULL;

	return CATS_SUCCESS;
}

int cats_packet_destroy(cats_packet_t** pkt)
{
	if((*pkt) == NULL) {
		return CATS_SUCCESS;
	}

	cats_whisker_t* whisker = (*pkt)->whiskers;
	while(whisker != NULL) {
		cats_whisker_t* prev = whisker;
		whisker = whisker->next;

		if(prev->type == WHISKER_TYPE_ROUTE) {
			cats_route_destroy(&(prev->data.route));
		}
		free(prev);
	}
	(*pkt)->whiskers = NULL;
	free(*pkt);

	return CATS_SUCCESS;
}

uint16_t cats_packet_encode(const cats_packet_t* pkt, uint8_t* out)
{
	assert(out != NULL);
	assert(pkt != NULL);
	cats_whisker_t* whisker = pkt->whiskers;
	if(whisker == NULL) {
		throw(EMPTY_PACKET);
	}

	// 1. Encode whiskers
	size_t written = 0;
	while(whisker != NULL) {
		written += cats_whisker_encode(whisker, out + written);
		whisker = whisker->next;
	}

	// 2. CRC checksum
	const uint16_t crc = cats_crc16(out, written);
	out[written++] = crc;
	out[written++] = crc >> 8;

	// 3. Whiten
	cats_whiten(out, written);

	// 4. LDPC encode
	written = cats_ldpc_encode(out, written);

	// 5. Interleave
	cats_interleave(out, written);

	assert(written <= CATS_MAX_PKT_LEN);
	return written;
}

int cats_packet_decode(cats_packet_t* pkt, uint8_t* buf, size_t buf_len)
{
	assert(pkt != NULL);
	assert(buf != NULL);
	assert(buf_len <= CATS_MAX_PKT_LEN);
	int len = buf_len;

	// 1. Deinterleave
	cats_deinterleave(buf, len);

	// 2. LDPC decode
	len = cats_ldpc_decode(buf, len);
	if(len < 0) {
		throw(LDPC_DECODE_FAIL);
	}

	// 3. Dewhiten
	cats_whiten(buf, len);

	// 4. CRC checksum
	const uint16_t crc_actual = cats_crc16(buf, len - 2);
	const uint16_t crc_expect = (buf[len-1] << 8) | buf[len - 2];
	if(crc_actual != crc_expect)
		throw(INVALID_CRC);

	// 5. Whiskers
	for(int i = 0; i < len - 2; i += 2) {
		const uint8_t whisker_len = buf[i + 1];
		
		cats_whisker_t whisker;
		cats_whisker_decode(&buf[i], &whisker);
		cats_packet_add_whisker(pkt, &whisker);

		i += whisker_len;
	}

	return CATS_SUCCESS;
}

int cats_packet_add_whisker_data(cats_packet_t* pkt, cats_whisker_type_t type, const cats_whisker_data_t* whisker_data, int len)
{
	cats_whisker_t whisker;
	whisker.type = type;
	whisker.len = len;
	whisker.data = *whisker_data;

	return cats_packet_add_whisker(pkt, &whisker);
}

int cats_packet_add_whisker(cats_packet_t* pkt, const cats_whisker_t* whisker)
{
	assert(whisker != NULL);
	cats_whisker_t* node = pkt->whiskers;

	if(node != NULL) {
		while(node->next != NULL) {
			node = node->next;
		}
		node->next = cats_whisker_new();
		node = node->next;
	}
	else {
		node = cats_whisker_new();
		pkt->whiskers = node;
	}
	
	pkt->len += whisker->len + 2;
	pkt->numWhiskers++;

	memcpy(node, whisker, sizeof(cats_whisker_t));
	node->next = NULL;

	return CATS_SUCCESS;
}

int cats_packet_find_whiskers(const cats_packet_t* pkt, cats_whisker_type_t type, cats_whisker_t*** out)
{
	assert(out != NULL);
	cats_whisker_t* whisker = pkt->whiskers;
	if(whisker == NULL) {
		throw(EMPTY_PACKET);
	}

	// Count instances of whisker type
	int found = 0;
	while(whisker != NULL) {
		if(whisker->type == type) {
			found++;
		}
		whisker = whisker->next;
	}
	if(found <= 0) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_find_whiskers: no whiskers of specified type found!");
	}

	// Add matched whiskers to array
	(*out) = malloc(sizeof(cats_whisker_t*) * found);
	if((*out) == NULL) {
		throw(MALLOC_FAIL);
	}
	
	int idx = 0;
	whisker = pkt->whiskers;
	while(whisker != NULL) {
		if(whisker->type == type) {
			(*out)[idx++] = whisker;
		}
		whisker = whisker->next;
	}

	return idx;
}

bool cats_packet_should_digipeat(const cats_packet_t* pkt, const char* callsign, uint16_t ssid)
{
	assert(pkt != NULL);
	cats_route_whisker_t* route;
	int r = cats_packet_get_route(pkt, &route);
	if(r == CATS_FAIL) {
		return false; // No route found; don't digipeat
	}

	// Check if we are the next future hop in the route, also count the number of felinet hops
	int felinet_hops = 0;
	cats_route_hop_t* hop = &(route->hops);
	while(hop != NULL) {
		if(hop->hop_type == CATS_ROUTE_FUTURE) {
			if(strcmp(hop->callsign, callsign) == 0 && hop->ssid == ssid) {
				return true; // We are the first future hop; digipeat
			}
			return false; // We are not the first future hop; only that station can digipeat
		}
		else if(hop->hop_type == CATS_ROUTE_INET) {
			felinet_hops++;
		}
		else if(hop->hop_type == CATS_ROUTE_PAST) {
			if(strcmp(hop->callsign, callsign) == 0 && hop->ssid == ssid) {
				return false; // We've already digipeated this packet
			}
		}
		hop = hop->next;
	}

	if((route->num_hops - felinet_hops) < route->max_digipeats) {
		return true; // No future hops are specified and max_digipeats has not been reached; digipeat
	}

	return false; // No digipeat conditions have been met; don't digipeat
}

// https://stackoverflow.com/a/75139158
uint16_t cats_crc16(uint8_t* data, int len)
{
	uint16_t crc = 0xFFFF;
	for(int i = 0; i < len; i++) {
		crc ^= data[i];
		for(uint8_t j = 0; j < 8; j++)
			crc = (crc & 1) != 0 ? (crc >> 1) ^ 0x8408 : crc >> 1;
	}
	return ~crc;
}