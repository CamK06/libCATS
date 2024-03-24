#include "cats/whisker.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

size_t cats_route_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_route_whisker_t* route = &(data->route);
    int ptr = 2; // Skip over type and length
    dest[ptr++] = data->route.maxDigipeats;
    
    if(route->numHops >= 1) {
        const cats_route_hop_t* hop = &(route->hops);
        while(hop != NULL) {
            if(hop->hopType == CATS_ROUTE_INET) {
                dest[ptr++] = hop->hopType;
            }
            else {
                strcpy(&dest[ptr], hop->callsign);
                ptr += strlen(hop->callsign);
                dest[ptr++] = hop->hopType;
                dest[ptr++] = hop->ssid;
                if(hop->hopType == CATS_ROUTE_PAST)
                    dest[ptr++] = hop->rssi;
            }

            hop = hop->next;
        }
    }
    
    return ptr - 2;
}

void cats_route_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
    cats_route_whisker_t* route = &(dest->route);
    int delim = 0;
    int begin = 3;
    route->maxDigipeats = data[2];

    cats_route_hop_t* hop = NULL;
    for(int i = begin; i < len + 2; i++) {
        // If there is a valid delimiter in data[i]
        if(data[i] == CATS_ROUTE_FUTURE || data[i] == CATS_ROUTE_PAST || data[i] == CATS_ROUTE_INET) {
            delim = i;
            hop = cats_route_append_hop(route);
            hop->hopType = data[delim];
            
            if(hop->hopType != CATS_ROUTE_INET) {
                memcpy(hop->callsign, &data[begin], delim - begin);
                begin = delim + 2;
                hop->ssid = data[delim + 1];
                route->len += strlen(hop->callsign) + 1;
                if(data[delim] == CATS_ROUTE_PAST) {
                    hop->rssi = data[delim + 2];
                    route->len++;
                    begin++;
                }
            }
            else if(hop->hopType == CATS_ROUTE_INET) {
                begin++;
            }
        }
    }
}