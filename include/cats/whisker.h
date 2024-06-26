/** @file whisker.h
 * 
 * @brief Whisker handling
*/

#ifndef CATS_WHISKER_H
#define CATS_WHISKER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CATS_ROUTE_PAST 0xFF
#define CATS_ROUTE_FUTURE 0xFD
#define CATS_ROUTE_INET 0xFE

#define CATS_NODEINFO_HARDWARE_ID 1
#define CATS_NODEINFO_SOFTWARE_ID 2
#define CATS_NODEINFO_UPTIME 4
#define CATS_NODEINFO_ANTENNA_HEIGHT 8
#define CATS_NODEINFO_ANTENNA_GAIN 16
#define CATS_NODEINFO_TX_POWER 32
#define CATS_NODEINFO_VOLTAGE 64
#define CATS_NODEINFO_TEMP 128
#define CATS_NODEINFO_BATTERY 256
#define CATS_NODEINFO_ALTITUDE 512
#define CATS_NODEINFO_IS_BALLOON 1024
#define CATS_NODEINFO_AMBIENT_TEMP 2048
#define CATS_NODEINFO_AMBIENT_HUMIDITY 4096
#define CATS_NODEINFO_AMBIENT_PRESSURE 8192

#define CATS_MAX_WHISKER_LEN 255
#define CATS_MAX_WHISKERS 255

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
#define CATS_NUM_WHISKER_TYPES 10

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

struct nodeinfo_voltage {
	bool enabled;
	float val;
};

struct nodeinfo_u8 {
	bool enabled;
	uint8_t val;
};

struct nodeinfo_i8 {
	bool enabled;
	int8_t val;
};

struct nodeinfo_u16 {
	bool enabled;
	uint16_t val;
};

struct nodeinfo_i16 {
	bool enabled;
	int16_t val;
};

struct nodeinfo_u32 {
	bool enabled;
	uint32_t val;
};

struct nodeinfo_i32 {
	bool enabled;
	int32_t val;
};

struct nodeinfo_f32 {
	bool enabled;
	float val;
};

struct nodeinfo_bool {
	bool enabled;
};

typedef struct cats_ident_whisker {
	uint16_t icon;
	uint8_t callsign[252]; // 252 = 255 - icon - ssid
	uint8_t ssid;
} cats_ident_whisker_t;

typedef struct cats_gps_whisker {
	double latitude;
	double longitude;
	float altitude;
	uint8_t max_error;
	uint8_t heading;
	float speed;
} cats_gps_whisker_t;

typedef struct cats_route_hop {
	uint8_t hop_type;
	uint8_t ssid;
	float rssi;
	uint8_t callsign[16];
	struct cats_route_hop* next;
} cats_route_hop_t;

typedef struct cats_route_whisker {
	uint8_t max_digipeats;
	cats_route_hop_t hops;
	uint8_t num_hops;
	uint8_t len;
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

typedef struct cats_nodeinfo_whisker {
	struct nodeinfo_u16 hardware_id;
	struct nodeinfo_u8 software_id;
	struct nodeinfo_u32 uptime;
	struct nodeinfo_u8 ant_height;
	struct nodeinfo_u8 ant_gain;
	struct nodeinfo_u8 tx_power;
	struct nodeinfo_voltage voltage;
	struct nodeinfo_i8 temperature;
	struct nodeinfo_u8 battery_level;
	struct nodeinfo_f32 altitude;
	struct nodeinfo_bool is_balloon;
	struct nodeinfo_i8 ambient_temp;
	struct nodeinfo_u8 ambient_humidity;
	struct nodeinfo_u16 ambient_pressure;
} cats_nodeinfo_whisker_t;

typedef union cats_whisker_data {
	cats_ident_whisker_t identification;
	cats_gps_whisker_t gps;
	cats_route_whisker_t route;
	cats_destination_whisker_t destination;
	cats_simplex_whisker_t simplex;
	cats_repeater_whisker_t repeater;
	cats_nodeinfo_whisker_t node_info;
	uint8_t raw[255];
} cats_whisker_data_t;

typedef struct cats_whisker {
	uint8_t type;
	uint8_t len;
	cats_whisker_data_t data;
	struct cats_whisker* next;
} cats_whisker_t;

/** 
 * @brief Encode a whisker into a byte array
 * 
 * @param whisker The whisker to encode
 * @param out The byte array to write to
 * @return The number of bytes written to out
*/
size_t cats_whisker_encode(const cats_whisker_t* whisker, uint8_t* out);

/**
 * @brief Decode a whisker from a byte array
 * 
 * @param data The byte array to decode
 * @param out The whisker to write to
 * @return CATS_SUCCESS on success, CATS_FAIL on failure
*/
int cats_whisker_decode(const uint8_t* data, cats_whisker_t* out);

/**
 * @brief Get the length of a whisker base type
 * 
 * @param type The whisker type
 * @return The length of the whisker base type
*/
int cats_whisker_base_len(const cats_whisker_type_t type);

/** 
 * @brief Create a new whisker
 * 
 * @return A pointer to the new whisker
*/
cats_whisker_t* cats_whisker_new();

/** 
 * @brief Add a future hop to a route whisker
 * 
 * @param route The route whisker to add to
 * @param callsign The callsign of the hop
 * @param ssid The SSID of the hop
 * @return A pointer to the new hop
*/
cats_route_hop_t* cats_route_add_future_hop(cats_route_whisker_t* route, const char* callsign, uint8_t ssid);

/** 
 * @brief Add a past hop to a route whisker
 * 
 * @param route The route whisker to add to
 * @param callsign The callsign of the hop
 * @param ssid The SSID of the hop
 * @param rssi The RSSI of the hop
 * @return A pointer to the new hop
*/
cats_route_hop_t* cats_route_add_past_hop(cats_route_whisker_t* route, const char* callsign, uint8_t ssid, float rssi);

/** 
 * @brief Add an internet hop to a route whisker
 * 
 * @param route The route whisker to add to
 * @return A pointer to the new hop
*/
cats_route_hop_t* cats_route_add_inet_hop(cats_route_whisker_t* route);

/** 
 * @brief Destroy a route whisker
 * 
 * @param route The route whisker to destroy
 * @note This function must be used rather than free()
*/
void cats_route_destroy(cats_route_whisker_t* route);

/** 
 * @brief Create a new route whisker
 * 
 * @param max_digipeats The maximum number of digipeats
 * @return The new route whisker
*/
cats_route_whisker_t cats_route_new(uint8_t max_digipeats);

#endif // CATS_WHISKER_H