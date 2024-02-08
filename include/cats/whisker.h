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
	WHISKER_TYPE_REPEATER,
	WHISKER_TYPE_NODEINFO
} cats_whisker_type_t;

typedef enum cats_modulation {
	MOD_UNK,
	MOD_CATS,
	MOD_FM,
	MOD_AM,
	MOD_USB,
	MOD_LSB,
	MOD_CW,
	MOD_FREEDV,
	MOD_M17,
	MOD_DSTAR,
	MOD_DMR,
	MOD_FUSION,
	MOD_P25
} cats_modulation_t;

typedef struct cats_ident_whisker {
	uint16_t icon;
	uint8_t callsign[252]; // 252 = 255 - icon - ssid
	uint8_t ssid;
} cats_ident_whisker_t;

typedef struct cats_gps_whisker {
	double latitude;
	double longitude;
	float altitude;
	uint8_t maxError;
	uint8_t heading;
	float speed;
} cats_gps_whisker_t;

typedef struct cats_route_whisker {
	uint8_t maxDigipeats;
	uint8_t routeData[254];
} cats_route_whisker_t;

typedef struct cats_destination_whisker {
	uint8_t ack;
	uint8_t callsign[253];
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
	double latitude;
	double longitude;
	uint8_t name[238];
} cats_repeater_whisker_t;

typedef struct cats_node_info_whisker {
	uint32_t bitmap;
	uint8_t data[251];
} cats_node_info_whisker_t;

typedef union cats_whisker_data {
	cats_ident_whisker_t identification;
	cats_gps_whisker_t gps;
	cats_route_whisker_t route;
	cats_destination_whisker_t destination;
	cats_simplex_whisker_t simplex;
	cats_repeater_whisker_t repeater;
	cats_node_info_whisker_t node_info;
	uint8_t raw[255];
} cats_whisker_data_t;

typedef struct cats_whisker {
	uint8_t type;
	uint8_t len;
	cats_whisker_data_t data;
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

int cats_whisker_base_len(cats_whisker_type_t type);

#endif
