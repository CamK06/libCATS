/** @file packet.h
 * 
 * @brief Packet handling; the core of the library
*/

#ifndef CATS_PACKET_H
#define CATS_PACKET_H

#include "whisker.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CATS_MAX_PKT_LEN 8191

/**
 * @brief CATS packet structure
 * @note Must be initialized and deinitialized using `cats_packet_prepare` and `cats_packet_destroy`
 */
typedef struct cats_packet_t {
    uint8_t len;
    uint8_t num_whiskers;
    cats_whisker_t* whiskers;
} cats_packet_t;

/**
 * @brief Initialize a `cats_packet_t` pointer
 * 
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 * 
 * @note Must be called before any functions using `cats_packet_t`
 * @note `pkt` MUST NOT already be allocated, otherwise it will leak memory
*/
int cats_packet_prepare(cats_packet_t** pkt);

/**
 * @brief Deinitialize a packet
 * 
 * @note NEVER call free() directly on a `cats_packet_t` pointer, this can result in memory leaks due to whiskers not being properly deallocated.
 * @note Always use this function to free a `cats_packet_t` pointer
 */
int cats_packet_destroy(cats_packet_t** pkt);

/**
 * @brief Semi-encode a CATS packet; only pass through the Whisker and CRC portion of the CATS pipeline. Used for FELINET and radio interfacing.
 * 
 * @param out Output buffer 
 * @return Length of data written to `out`
 */
uint16_t cats_packet_semi_encode(const cats_packet_t* pkt, uint8_t* out);

/**
 * @brief Encode a CATS packet
 * 
 * @param out Output buffer 
 * @return Length of data written to `out`
 */
uint16_t cats_packet_encode(const cats_packet_t* pkt, uint8_t* out);

/**
 * @brief Decode a semi-encoded CATS packet
 * 
 * @param pkt Packet to decode into
 * @param buf Input buffer
 * @param buf_len Length of input buffer
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 * 
 * @note `pkt` MUST be prepared before calling this function
 */
int cats_packet_semi_decode(cats_packet_t* pkt, uint8_t* buf, size_t buf_len);

/**
 * @brief Decode a CATS packet
 * 
 * @param pkt Packet to decode into
 * @param buf Input buffer
 * @param buf_len Length of input buffer
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 * 
 * @note `pkt` MUST be prepared before calling this function
 */
int cats_packet_decode(cats_packet_t* pkt, uint8_t* buf, size_t buf_len);

/**
 * @brief Add an identification whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param callsign Callsign
 * @param ssid SSID
 * @param icon Icon
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_identification(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint16_t icon);

/**
 * @brief Add a comment whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param comment Comment 
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_comment(cats_packet_t* pkt, const char* comment);

/**
 * @brief Add a GPS whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param lat Latitude 
 * @param lon Longitude
 * @param alt Altitude
 * @param error Error
 * @param heading Heading
 * @param speed Speed
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_gps(cats_packet_t* pkt, double lat, double lon, float alt, uint8_t error, uint8_t heading, float speed);

/**
 * @brief Add a route whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param route Route
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_route(cats_packet_t* pkt, cats_route_whisker_t route);

/**
 * @brief Add a destination whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param callsign Callsign
 * @param ssid SSID
 * @param ack Acknowledgement
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_destination(cats_packet_t* pkt, const char* callsign, uint8_t ssid, uint8_t ack);

/**
 * @brief Add a simplex whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param frequency Frequency
 * @param modulation Modulation
 * @param power Power
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_simplex(cats_packet_t* pkt, uint32_t frequency, cats_modulation_t modulation, uint8_t power);

/**
 * @brief Add a repeater whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param up Uplink frequency
 * @param down Downlink frequency
 * @param modulation Modulation
 * @param tone Tone
 * @param power Power
 * @param lat Latitude
 * @param lon Longitude
 * @param name Name
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_repeater(cats_packet_t* pkt, uint32_t up, uint32_t down, cats_modulation_t modulation, uint32_t tone, uint8_t power, double lat, double lon, const char* name);

/**
 * @brief Add a nodeinfo whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param info Nodeinfo
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_nodeinfo(cats_packet_t* pkt, cats_nodeinfo_whisker_t info);

/**
 * @brief Add arbitrary data to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param data Data
 * @param len Length of data
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_arbitrary(cats_packet_t* pkt, const uint8_t* data, size_t len);

/**
 * @brief Add raw whisker data to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param type Whisker type
 * @param whisker_data Whisker data
 * @param len Length of whisker data
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_whisker_data(cats_packet_t* pkt, cats_whisker_type_t type, const cats_whisker_data_t* whisker_data, int len);

/**
 * @brief Add a whisker to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param whisker Whisker to add
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 * 
 * @note Data is copied from the whisker into the packet, it is safe to free the whisker after calling this function
*/
int cats_packet_add_whisker(cats_packet_t* pkt, const cats_whisker_t* whisker);

/**
 * @brief Add a timestamp to a packet
 * 
 * @param pkt Packet to add whisker to
 * @param timestamp Timestamp
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_add_timestamp(cats_packet_t* pkt, uint64_t timestamp);

/**
 * @brief Get the identification whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_identification(const cats_packet_t* pkt, cats_ident_whisker_t** out);

/**
 * @brief Get the comment whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output comment
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_comment(const cats_packet_t* pkt, char* out);

/**
 * @brief Get the GPS whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_gps(const cats_packet_t* pkt, cats_gps_whisker_t** out);

/**
 * @brief Get the route whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_route(const cats_packet_t* pkt, cats_route_whisker_t** out);

/**
 * @brief Get the destination whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_destination(const cats_packet_t* pkt, cats_destination_whisker_t*** out);

/**
 * @brief Get the simplex whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_simplex(const cats_packet_t* pkt, cats_simplex_whisker_t*** out);

/**
 * @brief Get the repeater whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_repeater(const cats_packet_t* pkt, cats_repeater_whisker_t*** out);

/**
 * @brief Get arbitrary whiskers from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_arbitrary(const cats_packet_t* pkt, cats_whisker_t*** out);

/**
 * @brief Get the timestamp from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output timestamp
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
uint64_t cats_packet_get_timestamp(const cats_packet_t* pkt);

/**
 * @brief Get the nodeinfo whisker from a packet
 * 
 * @param pkt Packet to get whisker from
 * @param out Output whisker
 * @return `CATS_SUCCESS` on success, `CATS_FAIL` on failure
 */
int cats_packet_get_nodeinfo(const cats_packet_t* pkt, cats_nodeinfo_whisker_t** out);

/**
 * @brief Find whiskers of a certain type in a packet
 * 
 * @param pkt Packet to search
 * @param type Whisker type to search for
 * @param out Output array of whiskers
 * @return Number of whiskers found
 */
int cats_packet_find_whiskers(const cats_packet_t* pkt, cats_whisker_type_t type, cats_whisker_t*** out);

/**
 * @brief Check if a packet should be digipeated
 * 
 * @param pkt Packet to check
 * @param callsign Local Callsign
 * @param ssid Local SSID
 * @return `true` if the packet should be digipeated, `false` otherwise
 */
bool cats_packet_should_digipeat(const cats_packet_t* pkt, const char* callsign, uint16_t ssid);

#endif // CATS_PACKET_H