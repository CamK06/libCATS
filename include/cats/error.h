#ifndef CATS_ERROR_H
#define CATS_ERROR_H

#define CATS_FAIL 0
#define CATS_SUCCESS 1

// Maybe find a better way to do this? Feels wrong.
//#define throw(err) do { cats_throw_err(err, #err " " (__FILE__ + SOURCE_PATH_SIZE) __LINE__); return CATS_FAIL; } while(0)
#define throw(err) do { cats_throw_err(err, #err); return CATS_FAIL; } while(0)
#define throw_msg(err, msg) do { cats_throw_err(err, #err " " msg); return CATS_FAIL; } while(0)

/**
 * @brief Error codes for libCATS
 *
 * @note These are not to be used directly as return values; use throw() instead. Errors can be retrieved from cats_error.
*/
typedef enum {
    MALLOC_FAIL,
    UNSUPPORTED_WHISKER,
    DECODE_FAIL,
    PACKET_TOO_BIG,
    INVALID_OR_NO_CALLSIGN,
    INVALID_OR_NO_COMMENT,
    LDPC_DECODE_FAIL,
    INVALID_CRC,
    WHISKER_NOT_FOUND,
    EMPTY_PACKET,
    MAX_WHISKERS_OF_TYPE_EXCEEDED
} cats_error_t;

/**
 * @brief The most recent error thrown by libCATS
*/
extern cats_error_t cats_error;

/**
 * @brief Error string for the most recent error thrown by libCATS
*/
extern char cats_error_str[255];

/**
 * @brief Do NOT use this function directly, use throw() instead.
*/
void cats_throw_err(cats_error_t error, const char* msg);

#endif // CATS_ERROR_H