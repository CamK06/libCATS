#ifndef CATS_WHITENER_H
#define CATS_WHITENER_H

#include <stdint.h>
#include <stddef.h>

/**
* @brief Whiten/dewhiten the supplied data as per the CATS standard
*
* @param data Input and output buffer
* @param len Data length
*/
void cats_whiten(uint8_t* data, size_t len);

#endif // CATS_WHITENER_H