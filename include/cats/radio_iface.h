/** @file radio_iface.h
 * 
 * @brief Encoder and decoder for sending/receiving data to/from a CATS radio
*/

#ifdef BUILD_RADIO_IFACE
#ifndef CATS_RADIO_IFACE_H
#define CATS_RADIO_IFACE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Encode a packet with CBOR for transmission between a CATS radio and computer
 * 
 * @param buf Input and output buffer
 * @param buf_len Length of input data
 * @param rssi Receiver RSSI, if unknown or not applicable, set to 0
 * @return Encoded length
 */
uint16_t cats_radio_iface_encode(uint8_t* buf, const size_t buf_len, const float rssi);

/**
 * @brief Decode a packet with CBOR for transmission between a CATS radio and computer
 * 
 * @param buf Input and output buffer
 * @param buf_len Length of input data
 * @param rssi_out Pointer to store RSSI, set to NULL if not needed
 * @return Decoded length or CATS_FAIL on failure
 */
int cats_radio_iface_decode(uint8_t* buf, const size_t buf_len, float* rssi_out);

#endif // CATS_RADIO_IFACE_H
#endif // BUILD_RADIO_IFACE