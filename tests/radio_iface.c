/*
libCATS Radio Interface Test
*/

#include "cats/radio_iface.h"
#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <string.h>
#include <assert.h>

void test_encode_decode()
{
    // Prepare a test packet
    cats_packet_t* pkt;
    cats_packet_prepare(&pkt);
    cats_packet_add_identification(pkt, "VE3KCN", 7, 0);
    cats_packet_add_comment(pkt, "Testing 123 abc!");

    // Semi-Encode the CATS packet
    uint8_t buf[CATS_MAX_PKT_LEN];
    uint16_t len = cats_packet_semi_encode(pkt, buf);

    // Encode the CBOR data
    len = cats_radio_iface_encode(buf, len, -23.2f);
    cats_packet_destroy(&pkt);
    assert(len == 38);
    assert(pkt == NULL);
    
    // Decode the packet into pkt and rssi
    float rssi = 0;
    uint8_t decode_buf[CATS_MAX_PKT_LEN];
    len = cats_radio_iface_decode(buf, len, &rssi);
    assert(len > 0);
    assert((rssi - 23.2f) < 0.1f);

    // Decode the CATS packet
    cats_packet_prepare(&pkt);
    assert(cats_packet_semi_decode(pkt, buf, len) == CATS_SUCCESS);

    // Decode the identification
    cats_whisker_data_t* data;
    cats_packet_get_identification(pkt, (cats_ident_whisker_t**)&data);
    assert(strcmp(data->identification.callsign, "VE3KCN") == 0);
    assert(data->identification.ssid == 7);
    assert(data->identification.icon == 0);

    // Decode the comment
    char comment[1024];
    cats_packet_get_comment(pkt, comment);
    assert(strcmp(comment, "Testing 123 abc!") == 0);

    cats_packet_destroy(&pkt);
}

int main()
{
    test_encode_decode();
    return 0;
}