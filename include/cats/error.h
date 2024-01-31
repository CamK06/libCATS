#ifndef CATS_ERROR_H
#define CATS_ERROR_H

#define CATS_FAIL 0
#define CATS_SUCCESS 1

// Maybe find a better way to do this? Feels wrong.
#define throw(err, msg...) do { cats_throw_err(err, #err " " msg); return CATS_FAIL; } while(0)

typedef enum {
    MALLOC_FAIL,
    UNSUPPORTED_WHISKER,
    DECODE_FAIL,
    PACKET_TOO_BIG,
    INVALID_OR_NO_CALLSIGN,
    INVALID_OR_NO_COMMENT,
    LDPC_DECODE_FAIL,
    INVALID_CRC
} cats_error_t;

extern cats_error_t cats_error; // Most recent error thrown by libCATS
extern char cats_error_str[255]; // Error string for the most recent error thrown by libCATS

// Not to be used directly; use throw() instead
void cats_throw_err(cats_error_t error, const char* msg);

#endif