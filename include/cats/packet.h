#ifndef CATS_PACKET_H
#define CATS_PACKET_H

#include "whisker.h"
#include <stdint.h>

#define CATS_MAX_PKT_LEN 8191

typedef struct cats_packet_t {
    uint8_t len;
    uint8_t numWhiskers;
    cats_whisker_t* whiskers;
} cats_packet_t;

/*
Initialize a cats_packet_t pointer

Must be called before any functions using cats_packet_t
pkt MUST NOT already be allocated, otherwise it will leak memory
*/
int cats_packet_prepare(cats_packet_t** pkt);


/*
Build a cats_packet_t into a CATS packet for TX

Output buffer must be at least pkt->len bytes long
If output buffer is NULL, then it will be allocated according to the packet length

Returns the length of output on success; Returns CATS_FAIL on failure
*/
uint16_t cats_packet_build(cats_packet_t* pkt, uint8_t** out);


/* 
Decode a received packet into cats_packet_t

Returns 0 on success
*/
int cats_packet_from_buf(cats_packet_t* pkt, uint8_t* buf, uint16_t bufLen);


/*
Add an identification whisker to a packet

Returns 0 on success
*/
int cats_packet_add_identification(cats_packet_t* pkt, char* callsign, uint8_t ssid, uint16_t icon);


/*
Add a comment whisker to a packet

Returns 0 on success
*/
int cats_packet_add_comment(cats_packet_t* pkt, char* comment);


/*
Add a whisker to a packet

Note: whisker is deallocated after being copied

Returns 0 on success
*/
int cats_packet_add_whisker(cats_packet_t* pkt, cats_whisker_t** whisker);

/*
Get all instances of a whisker type in a packet

Returns array of whiskers; CATS_FAIL on fail
*/
cats_whisker_t** cats_packet_find_whiskers(cats_packet_t* pkt, cats_whisker_type_t type);

/*
Get the identification fields from a packet

Buffers must be pre-allocated, NULL fields will not be read

Returns 0 on success (at least one field has been read)
*/
int cats_packet_get_identification(cats_packet_t* pkt, char* callsign, uint8_t* ssid, uint16_t* icon);


/*
Get the comment from a packet

If comment buffer is NULL, it will be allocated according to the comment length
*/
int cats_packet_get_comment(cats_packet_t* pkt, char* comment);


// Everything below is NOT part of the 'public' API
// TODO: Make inaccessible?

/*
Encode a CATS packet from source 

Packet does not include header
dataOut must already be allocated

Returns the number of output bytes. Returns -1 if an error occurred.
*/
int cats_packet_encode(cats_whisker_t* whiskers, int whiskerCount, uint8_t** dataOut);

// Decode a CATS packet into its individual whiskers
// Packet must not include a header; Byte 0 should be the first byte after length L
// Returns the number of whiskers. Returns -1 if the packet is invalid.
int cats_packet_decode(uint8_t* data, int len, cats_whisker_t** whiskersOut);

// Calculate the CRC16/X25 of a buffer
uint16_t cats_crc16(uint8_t* data, int len);

#endif
