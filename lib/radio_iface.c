#ifdef BUILD_RADIO_IFACE
#include "cats/radio_iface.h"
#include "cats/error.h"
#include "cats/util.h"

#include "cbor/encoder.h"
#include "cbor/decoder.h"
#include "cbor/parser.h"

#include <assert.h>

uint16_t cats_radio_iface_encode(const cats_packet_t* pkt, const float rssi, uint8_t* out)
{
    assert(pkt != NULL);
    assert(out != NULL);

    uint8_t buf[CATS_MAX_PKT_LEN];
    const int len = cats_packet_semi_encode(pkt, buf);

    cbor_writer_t writer;
    cbor_writer_init(&writer, out, CATS_MAX_PKT_LEN);
    cbor_encode_byte_string(&writer, buf, len);
    cbor_encode_simple(&writer, cats_rssi_encode(rssi));

    return cbor_writer_len(&writer);
}

int cats_radio_iface_decode(cats_packet_t* pkt, const uint8_t* buf, const size_t buf_len, float* rssi_out)
{
    assert(pkt != NULL);
    assert(buf != NULL);

    cbor_item_t items[2];
    size_t n;

    cbor_reader_t reader;
    cbor_reader_init(&reader, items, sizeof(items) / sizeof(items[0]));
    cbor_error_t err = cbor_parse(&reader, buf, buf_len, &n);
    
    if(err != CBOR_SUCCESS || items[0].type != CBOR_ITEM_STRING 
    || items[1].type != CBOR_ITEM_SIMPLE_VALUE) {
        throw(DECODE_FAIL);
    }

    uint8_t pkt_buf[CATS_MAX_PKT_LEN];
    uint8_t rssi = 0;
    cbor_decode(&reader, &items[0], pkt_buf, CATS_MAX_PKT_LEN); // Semi-encoded packet
    if(rssi_out != NULL) {
        cbor_decode(&reader, &items[1], &rssi, 1); // RSSI
        (*rssi_out) = cats_rssi_decode(rssi);
    }

    return cats_packet_semi_decode(pkt, pkt_buf, items[0].size);
}

#endif // BUILD_RADIO_IFACE