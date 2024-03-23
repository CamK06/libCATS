/*
libCATS Whisker Test
*/

#include "cats/whisker.h"
#include "cats/error.h"
#include "cats/util.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "util.h"

void test_identification()
{
    cats_ident_whisker_t data;
    strcpy(data.callsign, "VE3KCN");
    data.ssid = 7;
    data.icon = 1;
    
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_IDENTIFICATION;
    whisker->len = 3+strlen("VE3KCN");
    whisker->data.identification = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.identification;
    assert(strcmp(data.callsign, "VE3KCN") == 0);
    assert(data.ssid == 7);
    assert(data.icon == 1);

    free(whisker);
}

void test_timestamp()
{
    uint64_t timestamp = 1706668404;
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_COMMENT;
    whisker->len = 5;
    memcpy(whisker->data.raw, &timestamp, 5);

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    uint64_t decodedTime = 0;
    memcpy(&decodedTime, whisker->data.raw, 5);
    assert(decodedTime == timestamp);

    free(whisker);
}

void test_gps()
{
    cats_gps_whisker_t data;
    data.altitude = 50.3f;
    data.heading = 8;
    data.speed = 69;
    data.maxError = 5;
    data.latitude = 47.573135;
    data.longitude = -53.556391;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_GPS;
    whisker->len = 14;
    whisker->data.gps = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.gps;
    assert(fabs(data.altitude-50.3f) <= 0.05f);
    assert(data.heading == 8);
    assert(fabs(data.speed-69) <= 0.05f);
    assert(data.maxError == 5);
    assert(fabs(data.latitude-47.573135) < 0.0000001);
    assert(fabs(data.longitude-(-53.556391)) <= 0.0000001);

    free(whisker);
}

void test_comment()
{
    static uint8_t text[] = { "This is some test text for the whisker comment field 1234567890!@#$^&*()" };
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_COMMENT;
    whisker->len = strlen(text);
    strcpy(whisker->data.raw, text);

    uint8_t* buf = malloc(whisker->len+2);
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);
    assert(strcmp(whisker->data.raw, text) == 0);

    free(whisker);
    free(buf);
}

void test_route()
{
    static uint8_t expect[] = { 0x04, 0x1b, 0x03, 0x56, 0x45, 0x33, 0x4b, 0x43, 0x4e, 0xff, 0x07, 0x10, 0x56, 0x45, 0x32, 0x44,
                         0x45, 0x46, 0xfd, 0xea, 0x56, 0x45, 0x33, 0x58, 0x59, 0x5a, 0xfd, 0x0e, 0xfe};
    cats_route_whisker_t data;
    data.maxDigipeats = 3;
    data.len = 0;
    data.numHops = 0;

    cats_route_add_hop(&data, "VE3KCN", 7, 0x10, CATS_ROUTE_PAST);
    cats_route_add_hop(&data, "VE2DEF", 234, 0, CATS_ROUTE_FUTURE);
    cats_route_add_hop(&data, "VE3XYZ", 14, 0, CATS_ROUTE_FUTURE);
    cats_route_add_hop(&data, "", 0, 0, CATS_ROUTE_INET);

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_ROUTE;
    whisker->data.route = data;
    whisker->len = data.len;

    uint8_t* buf = malloc(whisker->len+2);
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.route;
    assert(data.maxDigipeats == 3);
    assert(strcmp(data.hops->callsign, "VE3KCN") == 0);
    assert(data.hops->hopType == 0xFF);
    assert(data.hops->rssi == 0x10);
    assert(data.hops->ssid == 7);
    assert(strcmp(data.hops[1].callsign, "VE2DEF") == 0);
    assert(data.hops[1].hopType == CATS_ROUTE_FUTURE);
    assert(data.hops[1].ssid == 234);
    assert(strcmp(data.hops[2].callsign, "VE3XYZ") == 0);
    assert(data.hops[2].hopType == CATS_ROUTE_FUTURE);
    assert(data.hops[2].rssi == 0x00);
    assert(data.hops[2].ssid == 14);
    assert(data.hops[3].hopType == CATS_ROUTE_INET);
    assert(memcmp(buf, expect, whisker->len) == 0);

    free(whisker);
    free(buf);
}

void test_destination()
{
    cats_destination_whisker_t data;
    data.ack = 0xea;
    strcpy(data.callsign, "VE3KCN");
    data.ssid = 7;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_DESTINATION;
    whisker->len = strlen("VE3KCN")+2;
    whisker->data.destination = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.destination;
    assert(data.ack == 0xea);
    assert(strcmp(data.callsign, "VE3KCN") == 0);
    assert(data.ssid == 7);

    free(whisker);
}

void test_simplex()
{
    cats_simplex_whisker_t data;
    data.frequency = 146520000;
    data.modulation = 1;
    data.power = 5;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_SIMPLEX;
    whisker->len = 6;
    whisker->data.simplex = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.simplex;
    assert(data.frequency == 146520000);
    assert(data.modulation == 1);
    assert(data.power == 5);

    free(whisker);
}

void test_repeater()
{
    cats_repeater_whisker_t data;
    data.downlink = 146520000;
    data.uplink = 147520000;
    data.latitude = 1;
    data.longitude = 2;
    data.modulation = 1;
    data.power = 200;
    data.tone = 1;
    strcpy(data.name, "VE3KCN-R");

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_REPEATER;
    whisker->len = strlen("VE3KCN-R")+17;
    whisker->data.repeater = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.repeater;
    assert(data.downlink == 146520000);
    assert(data.uplink == 147520000);
    assert(fabs(data.latitude-1) < 0.001);
    assert(fabs(data.longitude-2) < 0.001);
    assert(data.modulation == 1);
    assert(data.power == 200);
    assert(data.tone == 1);
    assert(strcmp(data.name, "VE3KCN-R") == 0);

    free(whisker);
}

int main()
{
    test_identification();
    test_timestamp();
    test_gps();
    test_comment();
    test_route();
    test_destination();
    test_simplex();
    test_repeater();
    return 0;
}