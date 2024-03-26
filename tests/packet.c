/*
libCATS Packet Test
*/

#include "cats/packet.h"
#include "cats/error.h"
#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

extern uint16_t cats_crc16(uint8_t* data, int len);

void test_crc16()
{
    static uint8_t buf[] = {0xde, 0xea, 0xdb, 0xee, 0xff};
    const uint16_t crc = cats_crc16(buf, 5);
    assert(crc == 0xe384);
}

void test_decode()
{
    static uint8_t buf[] = {
        0x42, 0x00, 0xa7, 0x57, 0x7e, 0x2b, 0xad, 0xcd, 0xb0, 0x6c, 0xe5, 0x0d,
        0x51, 0x7a, 0x02, 0xce, 0xe5, 0xa2, 0x40, 0xa9, 0x20, 0x1f, 0x83, 0x9a,
        0xcf, 0x46, 0x4b, 0x40, 0x7b, 0x75, 0xd4, 0x40, 0x2d, 0xcc, 0x21, 0x80,
        0xae, 0xde, 0x5d, 0x69, 0x9d, 0xfd, 0x0a, 0x14, 0x8f, 0x35, 0x5b, 0x68,
        0xe9, 0x3c, 0x8f, 0x54, 0x2a, 0x46, 0xb1, 0xbc, 0xc5, 0x41, 0x6c, 0x50,
        0xdf, 0xbd, 0xd5, 0xa1, 0xe1, 0xdd, 0x30, 0x34
    };

    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);
    int r = cats_packet_decode(pkt, buf, 68);
    
    assert(r == CATS_SUCCESS);
}

void test_encode_decode()
{
    // Create a test packet
    uint8_t* buf = calloc(CATS_MAX_PKT_LEN, 1);
    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);
    cats_packet_add_identification(pkt, "VE3KCN", 7, 1);
    cats_packet_add_comment(pkt, "Hello libCATS world!");
    cats_packet_add_destination(pkt, "VE3KCN", 5, 0);
    cats_packet_add_gps(pkt, 47.573135, -53.556391, 50.3f, 5, 8, 69.0f);
    cats_packet_add_repeater(pkt, 147520000, 146520000, MOD_FM, 1, 200, 47.57, -53.55, "VE3KCN-R");
    cats_packet_add_simplex(pkt, 14652000, MOD_FM, 5);
    //cats_packet_add_node_info();
    //cats_packet_add_route();
    int len = cats_packet_encode(pkt, buf);
    assert(len > 0);
    cats_packet_destroy(&pkt);

    // Decode the test packet
    cats_packet_prepare(&pkt);
    int r = cats_packet_decode(pkt, buf, len);
    assert(r == CATS_SUCCESS);

    // Decode the identification
    cats_whisker_data_t* data;
    cats_packet_get_identification(pkt, (cats_ident_whisker_t**)&data);
    assert(strcmp(data->identification.callsign, "VE3KCN") == 0);
    assert(data->identification.ssid == 7);
    assert(data->identification.icon == 1);

    // Decode the comment
    char comment[1024];
    cats_packet_get_comment(pkt, comment);
    assert(strcmp(comment, "Hello libCATS world!") == 0);

    // Decode destination
    cats_destination_whisker_t** destinations;
    cats_packet_get_destination(pkt, &destinations);
    assert(strcmp(destinations[0]->callsign, "VE3KCN") == 0);
    assert(destinations[0]->ssid == 5);
    assert(destinations[0]->ack == 0);
    free(destinations);

    // Decode GPS
    cats_packet_get_gps(pkt, (cats_gps_whisker_t**)&data);
    assert(fabs(data->gps.altitude-50.3f) <= 0.05f);
    assert(data->gps.heading == 8);
    assert(fabs(data->gps.speed-69) <= 0.05f);
    assert(data->gps.max_error == 5);
    assert(fabs(data->gps.latitude-47.573135) < 0.0000001);
    assert(fabs(data->gps.longitude-(-53.556391)) <= 0.0000001);

    // Decode repeater
    cats_repeater_whisker_t** repeater;
    cats_packet_get_repeater(pkt, &repeater);
    assert(repeater[0]->downlink == 146520000);
    assert(repeater[0]->uplink == 147520000);
    assert(fabs(repeater[0]->latitude-47.57) < 0.01);
    assert(fabs(repeater[0]->longitude-(-53.55)) <= 0.01);
    assert(repeater[0]->modulation == MOD_FM);
    assert(repeater[0]->power == 200);
    assert(repeater[0]->tone == 1);
    assert(strcmp(repeater[0]->name, "VE3KCN-R") == 0);
    free(repeater);

    // Decode simplex
    cats_simplex_whisker_t** simplex;
    cats_packet_get_simplex(pkt, &simplex);
    assert(simplex[0]->frequency == 14652000);
    assert(simplex[0]->modulation == MOD_FM);
    assert(simplex[0]->power == 5);
    free(simplex);

    cats_packet_destroy(&pkt);
    free(buf);
}

void test_long_comment()
{
    uint8_t* comment = malloc(512);
    uint8_t* expect = malloc(512);
    uint8_t text[] = "This is a test of a really long comment that requires splitting into two separate comment whiskers aaabbcccdeeeefff";
    for(int i = 0; i < 3; i++) {
        memcpy(comment + (i * strlen(text)), text, strlen(text));
        memcpy(expect + (i * strlen(text)), text, strlen(text));
    }

    uint8_t* buf = calloc(CATS_MAX_PKT_LEN, 1);
    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);
    cats_packet_add_comment(pkt, comment);

    int len = cats_packet_encode(pkt, buf);
    memset(comment, 0x00, 512);
    cats_packet_destroy(&pkt);
    cats_packet_prepare(&pkt);
    assert(len > 0);

    assert(cats_packet_decode(pkt, buf, len) == CATS_SUCCESS);
    assert(strcmp(comment, expect) != 0);
    cats_packet_get_comment(pkt, comment);
    assert(strcmp(comment, expect) == 0);

    cats_packet_destroy(&pkt);
    free(buf);
}

void test_should_digipeat()
{
    const char* callsign = "VE3KCN";
    const uint16_t ssid = 7;

    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);

    assert(!cats_packet_should_digipeat(pkt, callsign, 7)); // No route; don't digipeat

    cats_route_whisker_t new_route = cats_route_new(3);
    cats_packet_add_route(pkt, new_route);

    assert(cats_packet_should_digipeat(pkt, callsign, 7)); // max_digipeats < num_hops; digipeat

    cats_route_whisker_t* route = NULL;
    cats_packet_get_route(pkt, &route);
    assert(route != NULL);
    cats_route_add_inet_hop(route);
    cats_route_add_inet_hop(route);
    cats_route_add_inet_hop(route);
    cats_route_add_past_hop(route, "VE3AAA", 2, -20);

    assert(cats_packet_should_digipeat(pkt, callsign, 7)); // inet hops don't count; digipeat

    cats_route_add_future_hop(route, "VE3ABC", 1);
    cats_route_add_future_hop(route, callsign, ssid);

    assert(!cats_packet_should_digipeat(pkt, callsign, 7)); // We are not the next hop; don't digipeat
    assert(cats_packet_should_digipeat(pkt, "VE3ABC", 1)); // We are the next hop; digipeat

    cats_packet_destroy(&pkt);
}

int main()
{
    test_crc16();
    test_decode();
    test_encode_decode();
    test_long_comment();
    test_should_digipeat();
    return 0;
}