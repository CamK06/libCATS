/*
libCATS Packet Test
*/

#include "cats/packet.h"
#include <assert.h>

void test_crc16()
{
    uint8_t buf[] = {0xde, 0xea, 0xdb, 0xee, 0xff};
    uint16_t crc = cats_crc16(buf, 5);
    assert(crc == 0xe384);
}

int main()
{
    test_crc16();
    return -1;
}