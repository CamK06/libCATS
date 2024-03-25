#include "cats/whisker.h"
#include "cats/error.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

size_t cats_route_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
    const cats_route_whisker_t* route = &(data->route);
    int ptr = 2; // Skip over type and length
    dest[ptr++] = data->route.max_digipeats;
    
    if(route->num_hops >= 1) {
        const cats_route_hop_t* hop = &(route->hops);
        while(hop != NULL) {
            if(hop->hop_type == CATS_ROUTE_INET) {
                dest[ptr++] = hop->hop_type;
            }
            else {
                strcpy(&dest[ptr], hop->callsign);
                ptr += strlen(hop->callsign);
                dest[ptr++] = hop->hop_type;
                dest[ptr++] = hop->ssid;
                if(hop->hop_type == CATS_ROUTE_PAST)
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
    route->max_digipeats = data[2];

    cats_route_hop_t* hop = NULL;
    for(int i = begin; i < len + 2; i++) {
        // If there is a valid delimiter in data[i]
        if(data[i] == CATS_ROUTE_FUTURE || data[i] == CATS_ROUTE_PAST || data[i] == CATS_ROUTE_INET) {
            delim = i;
            hop = cats_route_append_hop(route);
            hop->hop_type = data[delim];
            
            if(hop->hop_type != CATS_ROUTE_INET) {
                memcpy(hop->callsign, &data[begin], delim - begin);
                begin = delim + 2; // Skip over SSID and delimiter itself
                hop->ssid = data[delim + 1];
                route->len += strlen(hop->callsign) + 1;
                if(data[delim] == CATS_ROUTE_PAST) {
                    hop->rssi = data[delim + 2];
                    route->len++;
                    begin++;
                }
            }
            else if(hop->hop_type == CATS_ROUTE_INET) {
                begin = delim + 1; // This *might* need to be begin++
            }
        }
    }
}

void cats_route_destroy(cats_route_whisker_t* route)
{
    cats_route_hop_t* hop = route->hops.next;
    while(hop != NULL) {
        cats_route_hop_t* prev = hop;
        hop = hop->next;
        free(prev);
    }
    route->hops.next = NULL;
}

cats_route_hop_t* cats_route_append_hop(cats_route_whisker_t* route)
{
    cats_route_hop_t* hop = &(route->hops);
    assert(hop != NULL);

    if(route->num_hops != 0) {
        while(hop->next != NULL) {
            hop = hop->next;
        }
        hop->next = cats_route_new_hop();
        hop = hop->next;
    }
    route->num_hops++;
    route->len++;

    assert(hop != NULL);
    return hop;
}

cats_route_hop_t* cats_route_new_hop()
{
    cats_route_hop_t* hop = calloc(sizeof(cats_route_hop_t), 1);
    if(hop == NULL) {
        throw(MALLOC_FAIL);
    }

    hop->hop_type = 0;
    hop->rssi = 0;
    hop->ssid = 0;
    hop->next = NULL;

    return hop;
}

cats_route_hop_t* cats_route_add_hop(cats_route_whisker_t* route, const char* callsign, uint8_t ssid, uint8_t rssi, uint8_t type)
{
    cats_route_hop_t* hop = cats_route_append_hop(route);
    assert(hop != NULL);

    hop->hop_type = type;
    hop->rssi = rssi;
    hop->ssid = ssid;
    strcpy(hop->callsign, callsign);

    if(type != CATS_ROUTE_INET) {
        route->len += 1 + strlen(callsign);
    }
    if(type == CATS_ROUTE_PAST) {
        route->len++;
    }

    return hop;
}