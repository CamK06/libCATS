#ifndef CATS_WHISKER_H
#define CATS_WHISKER_H

#include <stdint.h>

typedef enum cats_whisker_type {
	WHISKER_TYPE_IDENTIFICATION,
	WHISKER_TYPE_TIMESTAMP,
	WHISKER_TYPE_GPS,
	WHISKER_TYPE_COMMENT,
	WHISKER_TYPE_ROUTE,
	WHISKER_TYPE_DESTINATION,
	WHISKER_TYPE_ARBITRARY,
	WHISKER_TYPE_SIMPLEX,
	WHISKER_TYPE_REPEATER
} cats_whisker_type_t;

typedef struct cats_whisker {
	uint8_t type;
	uint8_t len;
	uint8_t* data;
} cats_whisker_t;

/*
Encode a whisker from cats_whisker_t

dataOut must already be allocated
*/
int cats_whisker_encode(cats_whisker_t* whisker, uint8_t* dataOut);

/*
Decodes a whisker from its raw bytes to its appropriate type

whisker must already be allocated
*/
int cats_whisker_decode(cats_whisker_t* whiskerOut, uint8_t* data);

typedef struct cats_ident_whisker {
	uint16_t icon;
	uint8_t* callsign;
	uint8_t ssid;	
} cats_ident_whisker_t;

typedef struct cats_gps_whisker {
	int32_t latitude;
	int32_t longitude;
	uint16_t altitude;
	uint8_t maxError;
	uint8_t heading;
	uint16_t speed;
} cats_gps_whisker_t;

typedef struct cats_route_whisker {
	uint8_t maxDigipeats;
	uint8_t* routeData;
} cats_route_whisker_t;

typedef struct cats_destination_whisker {
	uint8_t ack;
	uint8_t* callsign;
	uint8_t ssid;
} cats_destination_whisker_t;

typedef struct cats_simplex_whisker {
	uint32_t frequency;
	uint8_t modulation;
	uint8_t power;
} cats_simplex_whisker_t;

typedef struct cats_repeater_whisker {
	uint32_t uplink;
	uint32_t downlink;
	uint8_t modulation;
	uint32_t tone;
	uint8_t power;
	int16_t latitude;
	int16_t longitude;
	uint8_t* name;
} cats_repeater_whisker_t;

typedef struct cats_node_info_whisker {
	uint32_t bitmap;
	uint8_t* data;	
} cats_node_info_whisker_t;

#endif
