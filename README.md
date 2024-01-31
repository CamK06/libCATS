# libCATS
![Linux CI](https://github.com/CamK06/libCATS/actions/workflows/linux-build.yml/badge.svg)
![Windows CI](https://github.com/CamK06/libCATS/actions/workflows/windows-build.yml/badge.svg)
![CATS Tests](https://github.com/CamK06/libCATS/actions/workflows/cats-tests.yml/badge.svg)

## About

libCATS is an implementation of the [CATS](https://cats.radio/) standard in C, intended for embedded use in [firmware for CATS radios](https://github.com/CamK06/CATS-Firmware), as well as desktop CATS software. It provides a simple API for encoding and decoding CATS packets and their component whiskers. Software using libCATS does not have to be concerned with any 'low level' parts of the CATS standard, however those functions are still exposed in the API to allow for flexibility in software where it may be useful to not have everything abstracted.

libCATS aims to be 100% compliant with the CATS standard, and should work with existing CATS radios using the official [Rust implementation](https://gitlab.scd31.com/cats/ham-cats) of CATS.

## Examples
Note: API is subject to change, as it is early in development

Creating a simple packet with a callsign and comment:
```c
uint8_t* buf = NULL;
cats_packet_t* pkt;

cats_packet_prepare(&pkt); // Initialize pkt
cats_packet_add_identification(pkt, "VE3KCN", 7, 1); // Add an ID with callsign VE3KCN-7 and a logo of 1
cats_packet_add_comment(pkt, "Hello libCATS world!"); // Self explanatory; Add a comment whisker
int len = cats_packet_build(pkt, &buf); // Allocates buf automatically
free(pkt); // Packet builder no longer needed
```

Decoding a packet and reading the identification and comment fields:
```c
uint8_t* buf = ... // Buffer with the received packet
cats_packet_t* pkt;

cats_packet_prepare(&pkt);
if(!cats_packet_from_buf(pkt, buf, bufLen)) {
    fprintf(stderr, cats_error_str);
    return -1; // Decode failed
}

char comment[1024];
char callsign[255];
uint8_t ssid;
uint16_t icon;
cats_packet_get_identification(pkt, callsign, &ssid, &icon);
cats_packet_get_comment(pkt, comment);

free(buf);
free(pkt);
```

## Contributing
Pull requests are VERY much welcome! I'm still a novice when it comes to writing pure C, so there's a lot that can be improved.