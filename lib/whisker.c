#include "cats/whisker.h"
#include "cats/error.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int cats_whisker_encode(cats_whisker_t* whisker, uint8_t* dataOut)
{
    int len = whisker->len+2;
    uint8_t* out = malloc(len);
    if(out == NULL)
        throw(MALLOC_FAIL);

    out[0] = whisker->type;
    out[1] = whisker->len;
    switch(whisker->type) {
        case WHISKER_TYPE_IDENTIFICATION:
            cats_ident_whisker_t ident;
            memcpy(&ident, (void*)whisker->data, sizeof(cats_ident_whisker_t));
            memcpy(&out[2], &ident.icon, sizeof(uint16_t));
            memcpy(&out[4], ident.callsign, whisker->len-3);
            out[whisker->len+1] = ident.ssid;
        break;
        
        case WHISKER_TYPE_COMMENT:
        case WHISKER_TYPE_TIMESTAMP:
        case WHISKER_TYPE_ARBITRARY:
            memcpy(out+2, whisker->data, whisker->len);
        break;

        case WHISKER_TYPE_GPS:
            cats_gps_whisker_t gps;
            memcpy(&gps, (void*)whisker->data, sizeof(cats_gps_whisker_t));
            memcpy(&out[2], &gps.latitude, sizeof(int32_t));
            memcpy(&out[6], &gps.longitude, sizeof(int32_t));
            memcpy(&out[10], &gps.altitude, sizeof(uint16_t));
            memcpy(&out[12], &gps.maxError, sizeof(uint8_t));
            memcpy(&out[13], &gps.heading, sizeof(uint8_t));
            memcpy(&out[14], &gps.speed, sizeof(uint16_t));
        break;

        case WHISKER_TYPE_ROUTE:
            cats_route_whisker_t route;
            memcpy(&route, (void*)whisker->data, sizeof(cats_route_whisker_t));
            memcpy(&out[2], &route.maxDigipeats, sizeof(uint8_t));
            memcpy(&out[3], route.routeData, whisker->len-1);
        break;

        case WHISKER_TYPE_DESTINATION:
            cats_destination_whisker_t dest;
            memcpy(&dest, (void*)whisker->data, sizeof(cats_destination_whisker_t));
            memcpy(&out[2], &dest.ack, sizeof(uint8_t));
            memcpy(&out[3], dest.callsign, whisker->len-2);
            memcpy(&out[whisker->len+1], &dest.ssid, sizeof(uint8_t));
        break;

        case WHISKER_TYPE_SIMPLEX:
            cats_simplex_whisker_t simplex;
            memcpy(&simplex, (void*)whisker->data, sizeof(cats_simplex_whisker_t));
            memcpy(&out[2], &simplex.frequency, sizeof(uint32_t));
            memcpy(&out[6], &simplex.modulation, sizeof(uint8_t));
            memcpy(&out[7], &simplex.power, sizeof(uint8_t));
        break;

        case WHISKER_TYPE_REPEATER:
            cats_repeater_whisker_t repeater;
            memcpy(&repeater, (void*)whisker->data, sizeof(cats_repeater_whisker_t));
            memcpy(&out[2], &repeater.uplink, sizeof(uint32_t));
            memcpy(&out[6], &repeater.downlink, sizeof(uint32_t));
            memcpy(&out[10], &repeater.modulation, sizeof(uint8_t));
            memcpy(&out[11], &repeater.tone, 3);
            memcpy(&out[14], &repeater.power, sizeof(uint8_t));
            memcpy(&out[15], &repeater.latitude, sizeof(int16_t));
            memcpy(&out[17], &repeater.longitude, sizeof(int16_t));
            memcpy(&out[19], repeater.name, whisker->len-17);
        break;

        // Unsupported type
        default:
        free(out);
        throw(UNSUPPORTED_WHISKER);
    }

    memcpy(dataOut, out, len);
    free(out);
    return CATS_SUCCESS;
}

int cats_whisker_decode(cats_whisker_t* whiskerOut, uint8_t* data)
{
    cats_whisker_t* out = malloc(sizeof(cats_whisker_t));
    if(out == NULL)
        throw(MALLOC_FAIL);
    out->type = data[0];
	out->len = data[1];
    
    switch(out->type) {
        case WHISKER_TYPE_IDENTIFICATION:
            // Callsign
            cats_ident_whisker_t ident;
            ident.callsign = malloc(out->len-3);
            if(ident.callsign == NULL)
                throw(MALLOC_FAIL);
            memcpy(ident.callsign, &data[4], out->len-3);

            // SSID + Icon
            ident.ssid = data[out->len+1];
            memcpy(&ident.icon, &data[2], sizeof(uint16_t));

            // Copy to output
            out->data = malloc(sizeof(cats_ident_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&ident, sizeof(cats_ident_whisker_t));
        break;
        
        case WHISKER_TYPE_COMMENT:
        case WHISKER_TYPE_TIMESTAMP:
        case WHISKER_TYPE_ARBITRARY:
            out->data = malloc(out->len);
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, &data[2], out->len);
        break;

        case WHISKER_TYPE_GPS:
            cats_gps_whisker_t gps;
            memcpy(&gps.latitude, &data[2], sizeof(int32_t));
            memcpy(&gps.longitude, &data[6], sizeof(int32_t));
            memcpy(&gps.altitude, &data[10], sizeof(uint16_t));
            memcpy(&gps.maxError, &data[12], sizeof(uint8_t));
            memcpy(&gps.heading, &data[13], sizeof(uint8_t));
            memcpy(&gps.speed, &data[14], sizeof(uint16_t));

            // Copy to output
            out->data = malloc(sizeof(cats_gps_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&gps, sizeof(cats_gps_whisker_t));
        break;

        case WHISKER_TYPE_ROUTE:
            cats_route_whisker_t route;
            memcpy(&route.maxDigipeats, &data[2], sizeof(uint8_t));
            route.routeData = malloc(out->len-1);
            memcpy(route.routeData, &data[3], out->len-1);

            out->data = malloc(sizeof(cats_route_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&route, sizeof(cats_route_whisker_t));
        break;

        case WHISKER_TYPE_DESTINATION:
            cats_destination_whisker_t dest;
            memcpy(&dest.ack, &data[2], sizeof(uint8_t));
            memcpy(&dest.ssid, &data[out->len+1], sizeof(uint8_t));

            dest.callsign = malloc(out->len-2);
            if(dest.callsign == NULL)
                throw(MALLOC_FAIL);
            memcpy(dest.callsign, &data[3], out->len-2);

            out->data = malloc(sizeof(cats_destination_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&dest, sizeof(cats_destination_whisker_t));
        break;

        case WHISKER_TYPE_SIMPLEX:
            cats_simplex_whisker_t simplex;
            memcpy(&simplex.frequency, &data[2], sizeof(uint32_t));
            memcpy(&simplex.modulation, &data[6], sizeof(uint8_t));
            memcpy(&simplex.power, &data[7], sizeof(uint8_t));

            out->data = malloc(sizeof(cats_simplex_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&simplex, sizeof(cats_simplex_whisker_t));
        break;

        case WHISKER_TYPE_REPEATER:
            cats_repeater_whisker_t repeater;
            memcpy(&repeater.uplink, &data[2], sizeof(uint32_t));
            memcpy(&repeater.downlink, &data[6], sizeof(uint32_t));
            memcpy(&repeater.modulation, &data[10], sizeof(uint8_t));
            memcpy(&repeater.tone, &data[11], 3);
            memcpy(&repeater.power, &data[14], sizeof(uint8_t));
            memcpy(&repeater.latitude, &data[15], sizeof(int16_t));
            memcpy(&repeater.longitude, &data[17], sizeof(int16_t));
            repeater.name = malloc(out->len-17);
            if(repeater.name == NULL)
                throw(MALLOC_FAIL);
            memcpy(repeater.name, &data[19], out->len-17);

            out->data = malloc(sizeof(cats_repeater_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&repeater, sizeof(cats_repeater_whisker_t));
        break;

        // Unsupported type
        default:
            free(out);
            throw(UNSUPPORTED_WHISKER);
    }

    memcpy(whiskerOut, out, sizeof(cats_whisker_t));
    free(out);
    return CATS_SUCCESS;
}