#pragma once
#ifdef __cplusplus

extern "C" {
#include "cats/packet.h"
}
#include "cats/whisker.hpp"

#include <vector>

namespace CATS
{

class Packet
{
public:
    Packet() {
    }
    ~Packet() {
    }

    size_t encode(uint8_t* out) {
        cats_packet_prepare(&pkt);
        for(int i = 0; i < whiskers.size(); i++) {
            cats_packet_add_whisker_data(pkt, whiskers[i].get_type(), whiskers[i].get_data(), whiskers[i].get_len());
        }

        uint16_t len = cats_packet_encode(pkt, out);
        cats_packet_destroy(&pkt);
        return len;
    }

    int from_buf(uint8_t* buf, size_t len) {
        cats_packet_prepare(&pkt);
        cats_packet_decode(pkt, buf, len);
        cats_packet_destroy(&pkt);
        return cats_packet_decode(pkt, buf, len);
    }

    void push_whisker(Whisker whisker) {
        whiskers.push_back(whisker);
    }

private:
    cats_packet_t* pkt = nullptr;
    std::vector<Whisker> whiskers;
};

}

#endif // __cplusplus