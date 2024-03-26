#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <string.h>
#include <assert.h>

int cats_packet_get_arbitrary(const cats_packet_t* pkt, cats_whisker_t*** out)
{
    assert(out != NULL);
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_ARBITRARY, out);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_arbitrary: packet has no arbitrary whiskers!");
	}
	
	return whiskers_found;
}

int cats_packet_add_arbitrary(cats_packet_t* pkt, const uint8_t* data, size_t len)
{
    assert(pkt != NULL);
    assert(data != NULL);
    assert(len <= CATS_MAX_WHISKER_LEN);
	if(pkt->len + 2 + len > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}

	cats_whisker_t whisker;
    whisker.type = WHISKER_TYPE_ARBITRARY;
    whisker.len = len;
    memcpy(&(whisker.data), data, len);

	return cats_packet_add_whisker(pkt, &whisker);
}