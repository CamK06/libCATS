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

void test_identification()
{
    cats_ident_whisker_t* data = malloc(sizeof(cats_ident_whisker_t));
    data->callsign = malloc(strlen("VE3KCN"));
    strcpy(data->callsign, "VE3KCN");
    data->ssid = 7;
    data->icon = 1;
    
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_IDENTIFICATION;
    whisker->len = 3+strlen("VE3KCN");
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(data->callsign);
    free(data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_ident_whisker_t*)whisker->data;
    assert(strcmp(data->callsign, "VE3KCN") == 0);
    assert(data->ssid == 7);
    assert(data->icon == 1);

    free(data->callsign);
    free(data);
    free(whisker);
}

void test_timestamp()
{
    uint64_t timestamp = 1706668404;
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_COMMENT;
    whisker->len = 5;
    whisker->data = malloc(5);
    memcpy(whisker->data, &timestamp, 5);

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    uint64_t decodedTime = 0;
    memcpy(&decodedTime, whisker->data, 5);
    assert(decodedTime == timestamp);

    free(whisker->data);
    free(whisker);
}

void test_gps()
{
    cats_gps_whisker_t* data = malloc(sizeof(cats_gps_whisker_t));
    data->altitude = float32_to_float16(50.3f);
    data->heading = 8;
    data->speed = float32_to_float16(69);
    data->maxError = 5;
    data->latitude = 47573135;
    data->longitude = -53556391;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_GPS;
    whisker->len = 14;
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_gps_whisker_t*)whisker->data;
    assert(fabs(float16_to_float32(data->altitude)-50.3f) <= 0.05f);
    assert(data->heading == 8);
    assert(fabs(float16_to_float32(data->speed)-69) <= 0.05f);
    assert(data->maxError == 5);
    assert(data->latitude == 47573135);
    assert(data->longitude == -53556391);

    free(whisker->data);
    free(whisker);
}

void test_comment()
{
    uint8_t text[] = { "This is some test text for the whisker comment field 1234567890!@#$%^&*()" };
    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_COMMENT;
    whisker->len = strlen(text);
    whisker->data = malloc(strlen(text));
    strcpy(whisker->data, text);

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);
    assert(strcmp(whisker->data, text) == 0);

    free(whisker->data);
    free(whisker);
}

void test_route()
{
    uint8_t routeData[] = { 0xea, 0xff, 0x32 }; // TODO: Put real route data here 

    cats_route_whisker_t* data = malloc(sizeof(cats_route_whisker_t));
    data->maxDigipeats = 3;
    data->routeData = malloc(sizeof(routeData));
    memcpy(data->routeData, routeData, sizeof(routeData));

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_ROUTE;
    whisker->len = sizeof(routeData)+1;
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(data->routeData);
    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_route_whisker_t*)whisker->data;
    assert(data->maxDigipeats == 3);
    assert(memcmp(data->routeData, routeData, whisker->len-1) == 0);

    free(data->routeData);
    free(whisker->data);
    free(whisker);
}

void test_destination()
{
    cats_destination_whisker_t* data = malloc(sizeof(cats_destination_whisker_t));
    data->ack = 0xea;
    data->callsign = malloc(strlen("VE3KCN"));
    strcpy(data->callsign, "VE3KCN");
    data->ssid = 7;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_DESTINATION;
    whisker->len = strlen("VE3KCN")+2;
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(data->callsign);
    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_destination_whisker_t*)whisker->data;
    assert(data->ack == 0xea);
    assert(strcmp(data->callsign, "VE3KCN") == 0);
    assert(data->ssid == 7);

    free(data->callsign);
    free(whisker->data);
    free(whisker);
}

void test_simplex()
{
    cats_simplex_whisker_t* data = malloc(sizeof(cats_simplex_whisker_t));
    data->frequency = 146520000;
    data->modulation = 1;
    data->power = 5;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_SIMPLEX;
    whisker->len = 6;
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_simplex_whisker_t*)whisker->data;
    assert(data->frequency == 146520000);
    assert(data->modulation == 1);
    assert(data->power == 5);

    free(whisker->data);
    free(whisker);
}

void test_repeater()
{
    cats_repeater_whisker_t* data = malloc(sizeof(cats_repeater_whisker_t));
    data->downlink = 146520000;
    data->uplink = 147520000;
    data->latitude = 1;
    data->longitude = 2;
    data->modulation = 1;
    data->power = 200;
    data->tone = 1;
    data->name = malloc(strlen("VE3KCN-R"));
    strcpy(data->name, "VE3KCN-R");

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_REPEATER;
    whisker->len = strlen("VE3KCN-R")+17;
    whisker->data = (uint8_t*)data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) == CATS_SUCCESS);

    free(whisker->data);
    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(whisker, buf) == CATS_SUCCESS);

    data = (cats_repeater_whisker_t*)whisker->data;
    assert(data->downlink == 146520000);
    assert(data->uplink == 147520000);
    assert(data->latitude == 1);
    assert(data->longitude == 2);
    assert(data->modulation == 1);
    assert(data->power == 200);
    assert(data->tone == 1);
    assert(strcmp(data->name, "VE3KCN-R") == 0);

    free(whisker->data);
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