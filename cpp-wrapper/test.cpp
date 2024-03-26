#include <iostream>

#include "cats/whisker.hpp"
#include "cats/packet.hpp"

int main()
{
	CATS::Identification whisker("VE3KCN", 7, 5);
	CATS::Packet packet;
	packet.push_whisker(whisker);

	uint8_t buf[CATS_MAX_PKT_LEN];
	int len = packet.encode(buf);
	printf("Encoded Len: %d\n", len);
	for(int i = 0; i < len; i++) {
		printf("%X ", buf[i]);
	}
	printf("\n");
	return 0;
}
