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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

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
    data.max_error = 5;
    data.latitude = 47.573135;
    data.longitude = -53.556391;

    cats_whisker_t* whisker = malloc(sizeof(cats_whisker_t));
    whisker->type = WHISKER_TYPE_GPS;
    whisker->len = 14;
    whisker->data.gps = data;

    uint8_t buf[whisker->len+2];
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

    data = whisker->data.gps;
    assert(fabs(data.altitude-50.3f) <= 0.05f);
    assert(data.heading == 8);
    assert(fabs(data.speed-69) <= 0.05f);
    assert(data.max_error == 5);
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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);
    assert(strcmp(whisker->data.raw, text) == 0);

    free(whisker);
    free(buf);
}

void test_route()
{
    static uint8_t expect[] = { 0x4, 0x1b, 0x3, 0x56, 0x45, 0x33, 0x4b, 0x43, 0x4e, 0xff, 0x7, 0x88, 0x56, 0x45, 0x32, 0x44,
                                0x45, 0x46, 0xfd, 0xea, 0x56, 0x45, 0x33, 0x58, 0x59, 0x5a, 0xfd };
    cats_route_whisker_t data = cats_route_new(3);
    cats_route_add_past_hop(&data, "VE3KCN", 7, -69.0f);
    cats_route_add_future_hop(&data, "VE2DEF", 234);
    cats_route_add_future_hop(&data, "VE3XYZ", 14);
    cats_route_add_inet_hop(&data);

    cats_whisker_t* whisker = cats_whisker_new();
    whisker->type = WHISKER_TYPE_ROUTE;
    whisker->data.route = data;
    whisker->len = data.len+1;

    uint8_t* buf = malloc(whisker->len+2);
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);
    cats_route_destroy(&(whisker->data.route));
    free(whisker);

    whisker = cats_whisker_new();
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

    data = whisker->data.route;
    cats_route_hop_t* hop = &(data.hops);

    assert(data.max_digipeats == 3);
    assert(strcmp(hop->callsign, "VE3KCN") == 0);
    assert(hop->hop_type == 0xFF);
    assert(abs(hop->rssi-(-69.0f)) < 0.1f);
    assert(hop->ssid == 7);
    assert(hop->next != NULL);
    hop = hop->next;

    assert(strcmp(hop->callsign, "VE2DEF") == 0);
    assert(hop->hop_type == CATS_ROUTE_FUTURE);
    assert(hop->ssid == 234);
    assert(hop->next != NULL);
    hop = hop->next;

    assert(strcmp(hop->callsign, "VE3XYZ") == 0);
    assert(hop->hop_type == CATS_ROUTE_FUTURE);
    assert(hop->ssid == 14);
    assert(hop->next != NULL);
    hop = hop->next;

    assert(hop->hop_type == CATS_ROUTE_INET);
    assert(memcmp(buf, expect, whisker->len) == 0);

    cats_route_destroy(&(whisker->data.route));
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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

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
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);

    whisker = malloc(sizeof(cats_whisker_t));
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);

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

void test_nodeinfo()
{
    cats_nodeinfo_whisker_t info;
    info.ant_gain.enabled = false;
    info.ant_height.enabled = false;
    info.battery_level.enabled = false;
    info.hardware_id.enabled = true;
    info.software_id.enabled = false;
    info.temperature.enabled = false;
    info.tx_power.enabled = true;
    info.uptime.enabled = true;
    info.voltage.enabled = true;
    info.hardware_id.val = 7408;
    info.uptime.val = 98;
    info.tx_power.val = 30;
    info.voltage.val = 12.8;

    cats_whisker_t* whisker = cats_whisker_new();
    whisker->type = WHISKER_TYPE_NODEINFO;
    whisker->len = 16;
    whisker->data.node_info = info;

    uint8_t buf[whisker->len + 2];
    assert(cats_whisker_encode(whisker, buf) != CATS_FAIL);

    free(whisker);
    info.hardware_id.enabled = false;
    info.tx_power.enabled = false; // Just enough to make it fail if the decode didn't actually work

    whisker = cats_whisker_new();
    assert(cats_whisker_decode(buf, whisker) == CATS_SUCCESS);
    info = whisker->data.node_info;

    assert(info.ant_gain.enabled == false);
    assert(info.ant_height.enabled == false);
    assert(info.battery_level.enabled == false);
    assert(info.hardware_id.enabled == true);
    assert(info.software_id.enabled == false);
    assert(info.temperature.enabled == false);
    assert(info.tx_power.enabled == true);
    assert(info.uptime.enabled == true);
    assert(info.voltage.enabled == true);
    assert(info.hardware_id.val == 7408);
    assert(info.tx_power.val == 30);
    assert(info.voltage.val = 12.8);

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
    test_nodeinfo();
    return 0;
}
