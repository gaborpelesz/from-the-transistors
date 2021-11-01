#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#define UNIT_TESTING 1

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/cstring.h>
// #include <cutils/arrayi.h>

static void test_api_string_create(void **state) {
    struct string* str = string_create();

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");

    free(str->_s);
    free(str);
}

static void test_api_string_create_from(void **state) {
    struct string* str = string_create_from("Test string.");

    assert_int_equal(str->size, 12);

    // assuming growth factor == 2
    // and initial capacity == 12
    // then capacity must have increased from 12 to 24
    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(str->_capacity, 24);

    assert_string_equal(str->_s, "Test string.");

    string_destroy(str);
}

static void test_api_string_destroy(void **state) {
    // at this point string_create has been already tested
    struct string* str = string_create();
    struct string* str2 = string_create_from("Test string.");

    // cmocka will check for memory leaks
    string_destroy(str);
    string_destroy(str2);
}


static void test_internal_string_create_allocate(void **state) {
    struct string *str = _string_create_allocate(14);

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);

    // assuming growth factor == 2
    // and initial capacity == 12
    // then capacity must have increased from 12 to 24
    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(str->_capacity, 24);

    string_destroy(str);
}

static void test_internal_calc_capacity(void **state) {
    assert_int_equal(_string_calc_capacity(0), 12);
    assert_int_equal(_string_calc_capacity(5), 12);
    assert_int_equal(_string_calc_capacity(10), 12);

    assert_int_equal(_string_calc_capacity(11), 24);
    assert_int_equal(_string_calc_capacity(17), 24);
    assert_int_equal(_string_calc_capacity(22), 24);

    assert_int_equal(_string_calc_capacity(23), 48);
    assert_int_equal(_string_calc_capacity(36), 48);
    assert_int_equal(_string_calc_capacity(46), 48);

    assert_int_equal(_string_calc_capacity(47), 96);
    assert_int_equal(_string_calc_capacity(75), 96);
    assert_int_equal(_string_calc_capacity(94), 96);
}

static void test_internal_string_realloc(void **state) {
    struct string* str = *state;
    int retc;

    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);

    // GROWTH
    retc = _string_realloc(str, 16);
    str->size = 16;
    assert_int_equal(retc, _REALLOC_CHANGED);
    assert_int_equal(str->_capacity, 24);

    // GROWTH WITHIN LIMIT
    retc = _string_realloc(str, 18);
    str->size = 18;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 24);

    // SHRINK WITHIN LIMIT
    retc = _string_realloc(str, 9);
    str->size = 9;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 24);

    // SHRINK
    retc = _string_realloc(str, 8);
    str->size = 8;
    assert_int_equal(retc, _REALLOC_CHANGED);
    assert_int_equal(str->_capacity, 12);

    // NO CHANGE
    retc = _string_realloc(str, 4);
    str->size = 4;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 12);

    // finish
    *state = str;
}

static void test_api_string_copy(void **state) {
    struct string* dst = string_create_from("Test string.");

    const char* src = "A definitely new kind str.";
    unsigned int src_n = 26;

    string_copy(dst, src);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, 26);

    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2 * 2);
    assert_string_equal(dst->_s, src);

    string_destroy(dst);
}

static void test_api_string_empty(void **state) {
    struct string* str = string_create_from("Test string.");

    string_empty(str);

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");

    string_destroy(str);
}

static void test_api_string_append_chrlst(void **state) {
    struct string* dst = string_create_from("Test string.");

    unsigned int dst_n = dst->size;

    const char* src = "A definitely new kind str.";
    unsigned int src_n = 26;

    string_append_chrlst(dst, src);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, src_n + dst_n);

    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2 * 2);
    assert_string_equal(dst->_s, "Test string.A definitely new kind str.");

    string_destroy(dst);
}

static void test_api_string_append_chr(void **state) {
    struct string* dst = string_create_from("Test string");

    const char chr = '.';

    string_append_chr(dst, chr);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, 12);

    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2);
    assert_string_equal(dst->_s, "Test string.");

    string_destroy(dst);
}

static void test_api_string_at(void **state) {
    struct string* str = string_create_from("Test string.");
    
    assert_int_equal(string_at(str, 0), 'T');
    assert_int_equal(string_at(str, 5), 's');
    assert_int_equal(string_at(str, 9), 'n');

    string_destroy(str);
}

static void test_api_string_pop(void **state) {
    struct string* str = string_create_from("Test string.");

    unsigned int before_size = str->size;
    unsigned int before_capacity = str->_capacity;

    char popped = string_pop(str);

    printf("this: %c\n", popped);

    assert_int_equal(popped, '.');
    assert_int_equal(before_size-1, str->size);

    // assert capacity
    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(before_capacity, STRING_INITIAL_CAPACITY * STRING_GROWTH_FACTOR);

    string_pop(str);
    string_pop(str);
    string_pop(str);

    assert_int_equal(str->_capacity, STRING_INITIAL_CAPACITY);

    string_destroy(str);
}

/* -------- end of tests --------- */

static int setup_string_empty(void **state) {
    *state = string_create();
    return 0;
}

static int setup_string_teststr(void **state) {
    *state = string_create_from("Test string.");
    return 0;
}

static int teardown_string_destroy(void **state) {
    string_destroy(*state);
    return 0;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_internal_calc_capacity),
        cmocka_unit_test(test_api_string_create),
        cmocka_unit_test(test_api_string_create_from),
        cmocka_unit_test(test_api_string_destroy),
        cmocka_unit_test(test_internal_string_create_allocate),
        cmocka_unit_test_setup_teardown(test_internal_string_realloc, setup_string_empty, teardown_string_destroy),
        cmocka_unit_test(test_api_string_copy),
        cmocka_unit_test(test_api_string_empty),
        cmocka_unit_test(test_api_string_append_chrlst),
        cmocka_unit_test(test_api_string_append_chr),
        cmocka_unit_test(test_api_string_at),
        cmocka_unit_test(test_api_string_pop),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}