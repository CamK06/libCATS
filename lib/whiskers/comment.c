#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"

#include <string.h>
#include <stdlib.h>

int cats_packet_get_comment(const cats_packet_t* pkt, char* out)
{
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_COMMENT, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_comment: packet has no comment whiskers!");
	}

	cats_whisker_t* whisker = NULL;
	int written = 0;
	for(int i = 0; i < whiskers_found; i++) {
		whisker = whiskers[i];
		if(whisker->len <= 0) {
			throw_msg(INVALID_OR_NO_COMMENT, "cats_packet_get_comment: comment whisker length is <= 0!");
		}

		strncpy(out + written, whisker->data.raw, whisker->len);
		written += whisker->len;
	}
	out[written] = '\0';
	free(whiskers);

	return CATS_SUCCESS;
}

int cats_packet_add_comment(cats_packet_t* pkt, const char* comment)
{
	if(comment == NULL || strlen(comment) <= 0) {
		throw(INVALID_OR_NO_COMMENT);
	}
	if(pkt->len + 2 + strlen(comment) > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}

	cats_whisker_t whisker;
	whisker.type = WHISKER_TYPE_COMMENT;

	const int len = strlen(comment);
	int written = 0;
	int r = CATS_SUCCESS;
	while(written < len) {
		if((len - written) > 255) {
			whisker.len = 255;
		}
		else {
			whisker.len = len - written;
		}

		memset(whisker.data.raw, 0x00, 255);
		strncpy(whisker.data.raw, comment + written, whisker.len);

		r = cats_packet_add_whisker(pkt, &whisker);
		if(r == CATS_FAIL) {
			return r;
		}
		written += whisker.len;
	}

	return r;
}