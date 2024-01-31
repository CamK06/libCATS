/*
libCATS General Test

Test all of libCATS' functionality; use all components like any regular program would
*/

#include "cats/error.h"

#include <string.h>
#include <assert.h>

int test_error()
{
    throw(MALLOC_FAIL);
}

int test_error_msg()
{
    throw(MALLOC_FAIL, "This is a test error message!");
}

int main()
{
    assert(test_error() == CATS_FAIL);
    assert(strcmp(cats_error_str, "MALLOC_FAIL ") == 0);
    assert(test_error_msg() == CATS_FAIL);
    assert(strcmp(cats_error_str, "MALLOC_FAIL This is a test error message!") == 0);

    return 0;
}