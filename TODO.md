# TODO

## General

* Create error handling system and update old code to use it
* Create UTF8 helper to translate between ASCII and UTF8
* Clean up library API (possibly move to own header, so only necessary functions are accessible)
* Create C++ wrapper
* Refactor EVERYTHING. This code SUCKS

## CATS Standard

#### Not Implemented
* LDPC* (need to roll own implementation)
* The following whisker types:
    Timestamp, GPS, Route, Destination, Arbitrary, Simplex, Repeater


#### Implemented
* CRC
* Whitener
* Interleaver
* Packet headers
* Whiskers* (not all types, but the boilerplate for whiskers is there)
* Identification whisker
* Comment whisker