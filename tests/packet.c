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

void test_crc16()
{
    uint8_t buf[] = {0xde, 0xea, 0xdb, 0xee, 0xff};
    uint16_t crc = cats_crc16(buf, 5);
    assert(crc == 0xe384);
}

void test_decode()
{
    uint8_t* buf = malloc(68);
    uint8_t orig[] = {
        0x42, 0x00, 0xa7, 0x57, 0x7e, 0x2b, 0xad, 0xcd, 0xb0, 0x6c, 0xe5, 0x0d,
        0x51, 0x7a, 0x02, 0xce, 0xe5, 0xa2, 0x40, 0xa9, 0x20, 0x1f, 0x83, 0x9a,
        0xcf, 0x46, 0x4b, 0x40, 0x7b, 0x75, 0xd4, 0x40, 0x2d, 0xcc, 0x21, 0x80,
        0xae, 0xde, 0x5d, 0x69, 0x9d, 0xfd, 0x0a, 0x14, 0x8f, 0x35, 0x5b, 0x68,
        0xe9, 0x3c, 0x8f, 0x54, 0x2a, 0x46, 0xb1, 0xbc, 0xc5, 0x41, 0x6c, 0x50,
        0xdf, 0xbd, 0xd5, 0xa1, 0xe1, 0xdd, 0x30, 0x34
    };
    memcpy(buf, orig, 68);

    int r = cats_packet_decode(buf+2, 68-2, NULL); // buf+2 is to skip length bytes
    assert(r == 2);

    free(buf);
}

void test_encode_decode()
{
    // Create a test packet
    uint8_t* buf = NULL;
    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);
    cats_packet_add_identification(pkt, "VE3KCN", 7, 1);
    cats_packet_add_comment(pkt, "Hello libCATS world!");
    int len = cats_packet_build(pkt, &buf);
    assert(len > 0);
    free(pkt);

    // Decode the test packet
    cats_packet_prepare(&pkt);
    int r = cats_packet_from_buf(pkt, buf, len);
    assert(r == CATS_SUCCESS);

    // Decode the identification whisker
    char callsign[255];
    uint8_t ssid;
    uint16_t icon;
    cats_packet_get_identification(pkt, callsign, &ssid, &icon);
    assert(strcmp(callsign, "VE3KCN") == 0);
    assert(ssid == 7);
    assert(icon == 1);

    // Decode the comment
    char comment[1024];
    cats_packet_get_comment(pkt, comment);
    assert(strcmp(comment, "Hello libCATS world!") == 0);

    free(pkt);
    free(buf);
}

int main()
{
    test_crc16();
    test_decode();
    test_encode_decode();
    return 0;
}