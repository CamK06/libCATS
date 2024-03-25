#include "cats/interleaver.h"
#include "cats/whitener.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/ldpc.h"
#include "cats/util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int cats_packet_prepare(cats_packet_t** pkt)
{
	*pkt = malloc(sizeof(cats_packet_t));
	if((*pkt) == NULL)
		throw(MALLOC_FAIL);
	
	(*pkt)->len = 0;
	(*pkt)->numWhiskers = 0;
	(*pkt)->whiskers = NULL;

	return CATS_SUCCESS;
}

int cats_packet_destroy(cats_packet_t** pkt)
{
	if((*pkt) == NULL)
		return CATS_SUCCESS;

	cats_whisker_t* whisker = (*pkt)->whiskers;
	while(whisker != NULL) {
		cats_whisker_t* prev = whisker;
		whisker = whisker->next;
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
	uint16_t crc_actual = cats_crc16(buf, len - 2);
	uint16_t crc_expect = (buf[len-1] << 8) | buf[len - 2];
	if(crc_actual != crc_expect)
		throw(INVALID_CRC);

	// 5. Whiskers
	for(int i = 0; i < len - 2; i += 2) {
		uint8_t whisker_len = buf[i + 1];
		
		cats_whisker_t whisker;
		cats_whisker_decode(&buf[i], &whisker);
		cats_packet_add_whisker(pkt, &whisker);

		i += whisker_len;
	}

	return CATS_SUCCESS;
}

int cats_packet_add_identification(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint16_t icon)
{
	if(callsign == NULL || strlen(callsign) <= 0 
	|| strlen(callsign) > 252)
		throw(INVALID_OR_NO_CALLSIGN);
	if(pkt->len + 5 + strlen(callsign) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

	cats_ident_whisker_t identification;
	identification.ssid = ssid;
	identification.icon = icon;
	strcpy(identification.callsign, callsign);

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_IDENTIFICATION, (cats_whisker_data_t*)&identification, 3 + strlen(callsign));
}

int cats_packet_add_comment(cats_packet_t* pkt, const char* comment)
{
	if(comment == NULL || strlen(comment) <= 0)
		throw(INVALID_OR_NO_COMMENT);
	if(strlen(comment) > 255)
		return -1; // TODO: Handle longer comments
	if(pkt->len + 2 + strlen(comment) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

	cats_whisker_t whisker;
	whisker.type = WHISKER_TYPE_COMMENT;
	whisker.len = strlen(comment);
	strcpy(whisker.data.raw, comment);

	return cats_packet_add_whisker(pkt, &whisker);
}

int cats_packet_add_gps(cats_packet_t* pkt, double lat, double lon, float alt, uint8_t error, uint8_t heading, float speed)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_GPS) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

	cats_gps_whisker_t gps;
	gps.altitude = alt;
	gps.heading = heading;
	gps.speed = speed;
	gps.maxError = error;
	gps.latitude = lat;
	gps.longitude = lon;

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_GPS, (cats_whisker_data_t*)&gps, cats_whisker_base_len(WHISKER_TYPE_GPS));
}

int cats_packet_add_route(cats_packet_t* pkt, cats_route_whisker_t route)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_ROUTE) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);
	
	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_ROUTE, (cats_whisker_data_t*)&route, cats_whisker_base_len(WHISKER_TYPE_ROUTE) + route.len);
}

int cats_packet_add_destination(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint8_t ack)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_DESTINATION) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

	cats_destination_whisker_t dest;
	dest.ack = ack;
	dest.ssid = ssid;
	strcpy(dest.callsign, callsign);

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_DESTINATION, (cats_whisker_data_t*)&dest, cats_whisker_base_len(WHISKER_TYPE_DESTINATION) + strlen(callsign));
}

int cats_packet_add_simplex(cats_packet_t* pkt, uint32_t frequency, cats_modulation_t modulation, uint8_t power)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_SIMPLEX) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

	cats_simplex_whisker_t simplex;
	simplex.frequency = frequency;
	simplex.modulation = modulation;
	simplex.power = power;

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_SIMPLEX, (cats_whisker_data_t*)&simplex, cats_whisker_base_len(WHISKER_TYPE_SIMPLEX));
}

int cats_packet_add_repeater(cats_packet_t* pkt, uint32_t up, uint32_t down, cats_modulation_t modulation, uint32_t tone, uint8_t power, double lat, double lon, const char* name)
{
	if(pkt->len + 2 + cats_whisker_base_len(WHISKER_TYPE_REPEATER) > CATS_MAX_PKT_LEN)
		throw(PACKET_TOO_BIG);

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

int cats_packet_add_node_info()
{
	// TODO
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

cats_whisker_t** cats_packet_find_whiskers(const cats_packet_t* pkt, cats_whisker_type_t type)
{
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
	cats_whisker_t** out = malloc(sizeof(cats_whisker_t*)*found);
	if(out == NULL)
		throw(MALLOC_FAIL);
	int idx = 0;

	whisker = pkt->whiskers;
	while(whisker != NULL) {
		if(whisker->type == type) {
			out[idx++] = whisker;
		}
		whisker = whisker->next;
	}

	return out;
}

int cats_packet_get_identification(const cats_packet_t* pkt, cats_ident_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_IDENTIFICATION);
	if(r <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_identification: packet has no identification whisker!");
	}
	cats_whisker_t* whisker = *r;
	free(r);
	*out = &whisker->data.identification;

	return CATS_SUCCESS;
}

int cats_packet_get_comment(const cats_packet_t* pkt, char* comment)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_COMMENT);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_comment: packet has no comment whiskers!");
	cats_whisker_t* whisker = *r; // TODO: Handle comments across multiple whiskers
	free(r);

	if(whisker->len <= 0)
		throw_msg(INVALID_OR_NO_COMMENT, "cats_packet_get_comment: comment whisker length is <= 0!");
	strcpy(comment, whisker->data.raw);

	return CATS_SUCCESS;
}

int cats_packet_get_gps(const cats_packet_t* pkt, cats_gps_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_GPS);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_gps: packet has no GPS whiskers!");
	cats_whisker_t* whisker = *r;
	free(r);

	*out = &whisker->data.gps;

	return CATS_SUCCESS;
}

int cats_packet_get_route(const cats_packet_t* pkt, cats_route_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_ROUTE);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_route: packet has no route whiskers!");
	cats_whisker_t* whisker = *r;
	free(r);
	
	*out = &whisker->data.route;

	return CATS_SUCCESS;
}

int cats_packet_get_destination(const cats_packet_t* pkt, cats_destination_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_DESTINATION);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_destination: packet has no destination whiskers!");
	cats_whisker_t* whisker = *r;
	free(r);
	
	*out = &whisker->data.destination;

	return CATS_SUCCESS;
}

int cats_packet_get_simplex(const cats_packet_t* pkt, cats_simplex_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_SIMPLEX);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_simplex: packet has no simplex whiskers!");
	cats_whisker_t* whisker = *r;
	free(r);

	*out = &whisker->data.simplex;

	return CATS_SUCCESS;
}

int cats_packet_get_repeater(const cats_packet_t* pkt, cats_repeater_whisker_t** out)
{
	cats_whisker_t** r = cats_packet_find_whiskers(pkt, WHISKER_TYPE_REPEATER);
	if(r <= CATS_FAIL)
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_repeater: packet has no repeater whiskers!");
	cats_whisker_t* whisker = *r;
	free(r);

	*out = &whisker->data.repeater;

	return CATS_SUCCESS;
}

int cats_packet_get_node_info()
{
	// TODO
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