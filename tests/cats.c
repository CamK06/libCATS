/*
libCATS General Test

Test all of libCATS' functionality; use all components like any regular program would
*/

#include "cats/error.h"
#include "cats/util.h"

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

void test_float16()
{
    float orig = 69.5;
    uint16_t f16 = float32_to_float16(orig);
    float new_val = float16_to_float32(f16);
    assert(orig == new_val);
}

int main()
{
    assert(test_error() == CATS_FAIL);
    assert(strcmp(cats_error_str, "MALLOC_FAIL ") == 0);
    assert(test_error_msg() == CATS_FAIL);
    assert(strcmp(cats_error_str, "MALLOC_FAIL This is a test error message!") == 0);
    test_float16();

    return 0;
}