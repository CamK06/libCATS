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
    uint8_t text[] = { "This is some test text for the whisker comment field 1234567890!@#$%^&*()" };
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_COMMENT;
    whisker->len = strlen(text);
    strcpy(whisker->data.raw, text);

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);
    assert(strcmp(whisker->data.raw, text) == 0);

    free(whisker);
}

void test_route()
{
    uint8_t routeData[] = { 0xea, 0xff, 0x32 }; // TODO: Put real route data here 

    cats_route_whisker_t data;
    data.maxDigipeats = 3;
    memcpy(data.routeData, routeData, sizeof(routeData));

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_ROUTE;
    whisker->len = sizeof(routeData)+1;
    whisker->data.route = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = whisker->data.route;
    assert(data.maxDigipeats == 3);
    assert(memcmp(data.routeData, routeData, whisker->len-1) == 0);

    free(whisker);
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
    //assert(fabs(data.latitude-1) < 0.0000001);
    //assert(fabs(data.longitude-2) < 0.0000001);
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