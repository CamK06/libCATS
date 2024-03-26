#ifndef CATS_PACKET_H
#define CATS_PACKET_H

#include "whisker.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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

// Returns number of bytes written
uint16_t cats_packet_encode(const cats_packet_t* pkt, uint8_t* out);
int cats_packet_decode(cats_packet_t* pkt, uint8_t* buf, size_t buf_len);

int cats_packet_add_identification(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint16_t icon);
int cats_packet_add_comment(cats_packet_t* pkt, const char* comment);
int cats_packet_add_gps(cats_packet_t* pkt, double lat, double lon, float alt, uint8_t error, uint8_t heading, float speed);
int cats_packet_add_route(cats_packet_t* pkt, cats_route_whisker_t route);
int cats_packet_add_destination(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint8_t ack);
int cats_packet_add_simplex(cats_packet_t* pkt, uint32_t frequency, cats_modulation_t modulation, uint8_t power);
int cats_packet_add_repeater(cats_packet_t* pkt, uint32_t up, uint32_t down, cats_modulation_t modulation, uint32_t tone, uint8_t power, double lat, double lon, const char* name);
int cats_packet_add_nodeinfo(cats_packet_t* pkt, cats_nodeinfo_whisker_t info);
int cats_packet_add_arbitrary(cats_packet_t* pkt, const uint8_t* data, size_t len);
int cats_packet_add_whisker_data(cats_packet_t* pkt, cats_whisker_type_t type, const cats_whisker_data_t* whisker_data, int len);
// Copies data from whisker into packet, it is safe to free() whisker after calling this
int cats_packet_add_whisker(cats_packet_t* pkt, const cats_whisker_t* whisker);
int cats_packet_add_timestamp(cats_packet_t* pkt, uint64_t timestamp);

int cats_packet_get_identification(const cats_packet_t* pkt, cats_ident_whisker_t** out);
int cats_packet_get_comment(const cats_packet_t* pkt, char* out);
int cats_packet_get_gps(const cats_packet_t* pkt, cats_gps_whisker_t** out);
int cats_packet_get_route(const cats_packet_t* pkt, cats_route_whisker_t** out);
int cats_packet_get_destination(const cats_packet_t* pkt, cats_destination_whisker_t*** out);
int cats_packet_get_simplex(const cats_packet_t* pkt, cats_simplex_whisker_t*** out);
int cats_packet_get_repeater(const cats_packet_t* pkt, cats_repeater_whisker_t*** out);
int cats_packet_get_arbitrary(const cats_packet_t* pkt, cats_whisker_t*** out);
uint64_t cats_packet_get_timestamp(const cats_packet_t* pkt);
int cats_packet_get_nodeinfo(const cats_packet_t* pkt, cats_nodeinfo_whisker_t** out);
// Returns number of whiskers found; adds pointers to found whiskers to out
int cats_packet_find_whiskers(const cats_packet_t* pkt, cats_whisker_type_t type, cats_whisker_t*** out);
bool cats_packet_should_digipeat(const cats_packet_t* pkt, const char* callsign, uint16_t ssid);

#endif // CATS_PACKET_H