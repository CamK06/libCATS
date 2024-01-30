#include "cats/interleaver.h"
#include "cats/whitener.h"
#include "cats/packet.h"
#include "cats/ldpc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>

void cats_packet_prepare(cats_packet_t** pkt)
{
	*pkt = malloc(sizeof(cats_packet_t));
	if((*pkt) == NULL)
		return;
	
	(*pkt)->len = 0;
	(*pkt)->numWhiskers = 0;
	(*pkt)->whiskers = NULL;	
}

uint16_t cats_packet_build(cats_packet_t* pkt, uint8_t** out)
{
	if(*out == NULL)
		if((*out = malloc(pkt->len)) == NULL)
			return -1;
	
	// TODO: Validate the packet before encoding (e.g. ensure only one ID whisker exists, packet isn't too long, etc)
	return cats_packet_encode(pkt->whiskers, pkt->numWhiskers, out);
}

int cats_packet_from_buf(cats_packet_t* pkt, uint8_t* buf, uint16_t bufLen)
{
	// Maybe this is where we should remove the length in the first two bytes?
	pkt->numWhiskers = cats_packet_decode(buf, bufLen, &pkt->whiskers);
	if(pkt->numWhiskers <= 0) // TODO: Handle no whiskers/decode fail error
		return -1;
	return 0;
}

int cats_packet_add_identification(cats_packet_t* pkt, char* callsign, uint8_t ssid, uint16_t icon)
{
	if(callsign == NULL || strlen(callsign) <= 0)
		return -1; // No callsign error
	if(strlen(callsign) > 252)
		return -1; // Callsign too long
	if(pkt->len+5+strlen(callsign) > CATS_MAX_PKT_LEN)
		return -1; // Packet would be too long; TODO: Throw error

	cats_ident_whisker_t* data = malloc(sizeof(cats_ident_whisker_t));
	if(data == NULL)
		return -1;
	data->ssid = ssid;
	data->icon = icon;
	data->callsign = malloc(strlen(callsign));
	memcpy(data->callsign, callsign, strlen(callsign));
	
	cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
	if(whisker == NULL)
		return -1;
	whisker->type = WHISKER_TYPE_IDENTIFICATION;
	whisker->len = 3+strlen(callsign);
	whisker->data = (uint8_t*)data;

	return cats_packet_add_whisker(pkt, &whisker);
}

int cats_packet_add_comment(cats_packet_t* pkt, char* comment)
{
	if(comment == NULL || strlen(comment) <= 0)
		return -1;
	if(strlen(comment) > 255)
		return -1; // TODO: Handle longer comments
	if(pkt->len+2+strlen(comment) > CATS_MAX_PKT_LEN)
		return -1; // Packet would be too long

	uint8_t* text = malloc(strlen(comment));
	strcpy(text, comment);

	cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
	if(whisker == NULL)
		return -1;
	whisker->type = WHISKER_TYPE_COMMENT;
	whisker->len = strlen(comment);
	whisker->data = text;

	return cats_packet_add_whisker(pkt, &whisker);
}

int cats_packet_add_whisker(cats_packet_t* pkt, cats_whisker_t** whisker)
{
	pkt->len += (*whisker)->len+2;
	pkt->numWhiskers++;
	if(pkt->whiskers == NULL) {
		pkt->whiskers = malloc(sizeof(cats_whisker_t));
		if(pkt->whiskers == NULL)
			return -1;
	}
	else {
		cats_whisker_t* tmp = realloc(pkt->whiskers, pkt->numWhiskers * sizeof *tmp);
		if(tmp == NULL)
			return -1;
		pkt->whiskers = tmp;
	}
	memcpy((void*)pkt->whiskers+(sizeof(cats_whisker_t)*(pkt->numWhiskers-1)), *whisker, sizeof(cats_whisker_t));
	free(*whisker);
	return 0;
}

int cats_packet_get_identification(cats_packet_t* pkt, char* callsign, uint8_t* ssid, uint16_t* icon)
{

}

int cats_packet_get_comment(cats_packet_t* pkt, char* comment)
{

}

int cats_packet_encode(cats_whisker_t* whiskers, int whiskerCount, uint8_t** dataOut)
{
	int len = 0;
	for(int i = 0; i < whiskerCount; i++)
		len += whiskers[i].len+2;
	uint8_t* out = malloc(len+2);
	if(out == NULL)
		return -1;

	int written = 0;
	for(int i = 0; i < whiskerCount; i++) {
		int whiskerLen = whiskers[i].len+2;
		uint8_t whisker[whiskerLen];
		cats_whisker_encode(&(whiskers[i]), whisker);
		memcpy(out+written, whisker, whiskerLen);
		written += whiskerLen;
	}

	uint16_t crc = cats_crc16(out, len);
	memcpy(out+len, &crc, sizeof(uint16_t));
	len += 2;

	cats_whiten(out, len);
	len = cats_ldpc_encode(&out, len);
	cats_interleave(out, len);
	
	uint8_t* tmp = realloc(*dataOut, len);
	if(tmp == NULL)
		return -1;
	*dataOut = tmp;
	memcpy(*dataOut, out, len);
	free(out);
	return len;
}

int cats_packet_decode(uint8_t* data, int len, cats_whisker_t** whiskersOut)
{	
	int pktLen = len;
	uint8_t* pkt = malloc(pktLen);
	memcpy(pkt, data, pktLen);

	cats_deinterleave(pkt, pktLen);
	pktLen = cats_ldpc_decode(&pkt, pktLen);
	if(pktLen < 0)
		return -1;
	cats_whiten(pkt, pktLen);

	uint16_t crcActual = cats_crc16(pkt, pktLen-2);
	uint16_t crcExpect = *(uint16_t*)&pkt[pktLen-2]; // Is this okay practice? Probably should use memcpy...
	if(crcActual != crcExpect)
		return -1; // TODO: Error handling

	int numWhiskers = 0;
	for(int i = 0; i < pktLen-2; i+=2) {
		uint8_t len = pkt[i+1];
		numWhiskers++;
		i += len;
	}

	if(whiskersOut != NULL) {
		cats_whisker_t* whiskers = malloc(numWhiskers * sizeof(cats_whisker_t));
		int idx = 0;
		for(int i = 0; i < numWhiskers; i++) {
			cats_whisker_decode(&whiskers[i], &pkt[idx]);
			idx += whiskers[i].len+2;
		}
		*whiskersOut = whiskers;
	}

	free(pkt);
	return numWhiskers;
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