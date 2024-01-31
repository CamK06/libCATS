#include "cats/whisker.h"
#include "cats/error.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int cats_whisker_encode(cats_whisker_t* whisker, uint8_t* dataOut)
{
    int len = whisker->len+2;
    uint8_t* out = malloc(len);
    if(out == NULL)
        throw(MALLOC_FAIL);

    out[0] = whisker->type;
    out[1] = whisker->len;
    switch(whisker->type) {
        case WHISKER_TYPE_IDENTIFICATION:
        cats_ident_whisker_t ident;
        memcpy(&ident, (void*)whisker->data, sizeof(cats_ident_whisker_t));
        memcpy(&out[2], &ident.icon, sizeof(uint16_t));
        memcpy(&out[4], ident.callsign, whisker->len-3);
        out[whisker->len+1] = ident.ssid;
        break;
        
        case WHISKER_TYPE_COMMENT:
        memcpy(out+2, whisker->data, whisker->len);
        break;

        // Unsupported type
        default:
        free(out);
        throw(UNSUPPORTED_WHISKER);
    }

    memcpy(dataOut, out, len);
    free(out);
    return CATS_SUCCESS;
}

int cats_whisker_decode(cats_whisker_t* whiskerOut, uint8_t* data)
{
    cats_whisker_t* out = malloc(sizeof(cats_whisker_t));
    if(out == NULL)
        throw(MALLOC_FAIL);
    out->type = data[0];
	out->len = data[1];
    
    switch(out->type) {
        case WHISKER_TYPE_IDENTIFICATION:
            // Callsign
            cats_ident_whisker_t ident;
            ident.callsign = malloc(out->len-3);
            if(ident.callsign == NULL)
                throw(MALLOC_FAIL);
            memcpy(ident.callsign, &data[4], out->len-3);

            // SSID + Icon
            ident.ssid = data[out->len+1];
            memcpy(&ident.icon, &data[2], sizeof(uint16_t));

            // Copy to output
            out->data = malloc(sizeof(cats_ident_whisker_t));
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, (void*)&ident, sizeof(cats_ident_whisker_t));
        break;
        
        case WHISKER_TYPE_COMMENT:
            out->data = malloc(out->len);
            if(out->data == NULL)
                throw(MALLOC_FAIL);
            memcpy(out->data, &data[2], out->len);
        break;

        // Unsupported type
        default:
            free(out);
            throw(UNSUPPORTED_WHISKER);
    }

    memcpy(whiskerOut, out, sizeof(cats_whisker_t));
    free(out);
    return CATS_SUCCESS;
}