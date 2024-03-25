#include "cats/error.h"

#include <string.h>
#include <assert.h>

cats_error_t cats_error;
char cats_error_str[255];

void cats_throw_err(cats_error_t error, const char* msg) 
{
    assert(strlen(msg) < 256);
    strcpy(cats_error_str, msg);
    cats_error = error;
}