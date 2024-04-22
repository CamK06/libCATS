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

	CATS::Packet decode;
	decode.from_buf(buf, len);
	CATS::Identification ident = decode.get_identification();
	std::cout << ident.get_callsign() << "-" << ident.get_ssid() << " [ICON: " << ident.get_icon() << "]" << std::endl;
	return 0;
}
