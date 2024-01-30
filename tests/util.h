// Common helper functions for tests; only really used for dev purposes

#ifndef CATS_TEST_UTIL_H
#define CATS_TEST_UTIL_H

#include <stdint.h>
#include <stdio.h>

void hexdump(uint8_t* data, int len) {
    for(int i = 0; i < len; i++) {
	    if(i % 16 == 0)
		    printf("\n%08x", i);
	    if(i % 8 == 0)
		    printf("  ");
            printf("%02X ", data[i]);
    }
    printf("\n");
}

#endif
