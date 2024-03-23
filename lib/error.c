#include "cats/error.h"

#include <string.h>
#include <assert.h>

CATSError cats_error;
char cats_error_str[255];

void cats_throw_err(CATSError error, const char* msg) 
{
    assert(strlen(msg) < 256);
    strcpy(cats_error_str, msg);
    cats_error = error;
}