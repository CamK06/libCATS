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
        for(Whisker whisker : whiskers) {
            cats_packet_add_whisker_data(pkt, whisker.get_type(), whisker.get_data(), whisker.get_len());
        }
        //for(int i = 0; i < whiskers.size(); i++) {
        //    cats_packet_add_whisker_data(pkt, whiskers[i].get_type(), whiskers[i].get_data(), whiskers[i].get_len());
        //}

        uint16_t len = cats_packet_encode(pkt, out);
        cats_packet_destroy(&pkt);
        return len;
    }

    int from_buf(uint8_t* buf, size_t len) {
        cats_packet_prepare(&pkt);
        cats_packet_decode(pkt, buf, len);

        cats_whisker_t* whisker = pkt->whiskers;
        if(whisker == NULL) {
            return 0;
        }
        while(whisker != NULL) {
            Whisker new_whisker(whisker->data, (cats_whisker_type_t)whisker->type, whisker->len);
            push_whisker(new_whisker);
            whisker = whisker->next;
        }

        cats_packet_destroy(&pkt);
        return 1;
    }

    Identification get_identification() {
        return (Identification)get_whisker(WHISKER_TYPE_IDENTIFICATION);
    }

    void push_whisker(Whisker whisker) {
        whiskers.push_back(whisker);
    }

private:
    Whisker get_whisker(cats_whisker_type_t type) {
        for(Whisker whisker : whiskers) {
            if(whisker.get_type() == type) {
                return whisker;
            }
        }
        return Whisker();
    }

    cats_packet_t* pkt = nullptr;
    std::vector<Whisker> whiskers;
};

}

#endif // __cplusplus