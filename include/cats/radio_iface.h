#ifdef BUILD_RADIO_IFACE
#ifndef CATS_RADIO_IFACE_H
#define CATS_RADIO_IFACE_H

#include "cats/packet.h"

uint16_t cats_radio_iface_encode(const cats_packet_t* pkt, const float rssi, uint8_t* out);
int cats_radio_iface_decode(cats_packet_t* pkt, const uint8_t* buf, const size_t buf_len, float* rssi_out);

#endif // CATS_RADIO_IFACE_H
#endif // BUILD_RADIO_IFACE