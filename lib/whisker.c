#include "cats/whisker.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void cats_whisker_encode(cats_whisker_t* whisker, uint8_t* dataOut)
{
    int len = whisker->len+2;
    uint8_t* out = malloc(len);
    if(out == NULL)
        return;

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
        dataOut = NULL;
        return;
        break;
    }

    memcpy(dataOut, out, len);
    free(out);
}

void cats_whisker_decode(cats_whisker_t* whiskerOut, uint8_t* data)
{
    cats_whisker_t* out = malloc(sizeof(cats_whisker_t));
    if(out == NULL)
        return;
    out->type = data[0];
	out->len = data[1];
    
    switch(out->type) {
        case WHISKER_TYPE_IDENTIFICATION:
        cats_ident_whisker_t ident;
        memcpy(&ident.icon, &data[2], sizeof(uint16_t));
        memcpy(ident.callsign, &data[4], out->len-3);
        ident.ssid = data[out->len+1];
        out->data = malloc(sizeof(cats_ident_whisker_t));
        memcpy(out->data, (void*)&ident, sizeof(cats_ident_whisker_t));
        break;
        
        case WHISKER_TYPE_COMMENT:
        out->data = malloc(out->len);
        memcpy(out->data, &data[2], out->len);
        break;

        // Unsupported type
        default:
        free(out);
        whiskerOut = NULL;
        return;
        break;
    }

    
	out->data = malloc(data[1]);
	memcpy(out->data, &data[2], data[1]);
    memcpy(whiskerOut, out, sizeof(cats_whisker_t));
    free(out);
}