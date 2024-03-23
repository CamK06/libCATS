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

int cats_packet_destroy(cats_packet_t** pkt);

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

int cats_packet_add_gps(cats_packet_t* pkt, double lat, double lon, float alt, uint8_t error, uint8_t heading, float speed);
int cats_packet_add_route(cats_packet_t* pkt, cats_route_whisker_t route);
int cats_packet_add_destination(cats_packet_t* pkt, uint8_t* callsign, uint8_t ssid, uint8_t ack);
int cats_packet_add_simplex(cats_packet_t* pkt, uint32_t frequency, cats_modulation_t modulation, uint8_t power);
int cats_packet_add_repeater(cats_packet_t* pkt, uint32_t up, uint32_t down, cats_modulation_t modulation, uint32_t tone, uint8_t power, double lat, double lon, uint8_t* name);
int cats_packet_add_node_info(); // TODO

/*
Add a whisker to a packet from its data (constructs remainder of whisker)

Note: whisker is deallocated after being copied

Returns 0 on success
*/
int cats_packet_add_whisker_data(cats_packet_t* pkt, cats_whisker_type_t type, cats_whisker_data_t* whiskerData, int len);

int cats_packet_add_whisker(cats_packet_t* pkt, cats_whisker_t* whisker);

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
int cats_packet_get_identification(cats_packet_t* pkt, cats_ident_whisker_t** out);


/*
Get the comment from a packet

If comment buffer is NULL, it will be allocated according to the comment length
*/
int cats_packet_get_comment(cats_packet_t* pkt, char* comment);

int cats_packet_get_gps(cats_packet_t* pkt, cats_gps_whisker_t** out);
int cats_packet_get_route(cats_packet_t* pkt, cats_route_whisker_t** out);
int cats_packet_get_destination(cats_packet_t* pkt, cats_destination_whisker_t** out);
int cats_packet_get_simplex(cats_packet_t* pkt, cats_simplex_whisker_t** out);
int cats_packet_get_repeater(cats_packet_t* pkt, cats_repeater_whisker_t** out);
int cats_packet_get_node_info(); // TODO

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
