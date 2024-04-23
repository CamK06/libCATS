#ifdef BUILD_RADIO_IFACE
#ifndef CATS_RADIO_IFACE_H
#define CATS_RADIO_IFACE_H

#include <stdint.h>
#include <stddef.h>

uint16_t cats_radio_iface_encode(uint8_t* buf, const size_t buf_len, const float rssi);
int cats_radio_iface_decode(uint8_t* buf, const size_t buf_len, float* rssi_out);

#endif // CATS_RADIO_IFACE_H
#endif // BUILD_RADIO_IFACE