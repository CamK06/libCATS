#ifdef BUILD_RADIO_IFACE
#include "cats/radio_iface.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/util.h"

#include "cbor/encoder.h"
#include "cbor/decoder.h"
#include "cbor/parser.h"

#include <assert.h>
#include <string.h>
#include <stdint.h>

uint16_t cats_radio_iface_encode(uint8_t* buf, const size_t buf_len, const float rssi)
{
    assert(buf != NULL);
    uint8_t tmp[CATS_MAX_PKT_LEN];

    cbor_writer_t writer;
    cbor_writer_init(&writer, tmp, CATS_MAX_PKT_LEN);
    cbor_encode_byte_string(&writer, buf, buf_len);
    cbor_encode_simple(&writer, cats_rssi_encode(rssi));

    size_t len = cbor_writer_len(&writer);
    memcpy(buf, tmp, len);
    return len;
}

int cats_radio_iface_decode(uint8_t* buf, const size_t buf_len, float* rssi_out)
{
    assert(buf != NULL);
    uint8_t tmp[CATS_MAX_PKT_LEN];
    cbor_item_t items[2];
    size_t n;

    cbor_reader_t reader;
    cbor_reader_init(&reader, items, sizeof(items) / sizeof(items[0]));
    cbor_error_t err = cbor_parse(&reader, buf, buf_len, &n);
    
    if(err != CBOR_SUCCESS || items[0].type != CBOR_ITEM_STRING 
    || items[1].type != CBOR_ITEM_SIMPLE_VALUE) {
        throw(DECODE_FAIL);
    }

    uint8_t rssi = 0;
    cbor_decode(&reader, &items[0], tmp, CATS_MAX_PKT_LEN); // Semi-encoded packet
    if(rssi_out != NULL) {
        cbor_decode(&reader, &items[1], &rssi, 1); // RSSI
        (*rssi_out) = cats_rssi_decode(rssi);
    }

    size_t len = items[0].size;
    memcpy(buf, tmp, len);
    return len;
}

#endif // BUILD_RADIO_IFACE