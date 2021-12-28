#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/string.h>

static void test_api_cutils_string_create(void **state) {
    struct cutils_string* str = cutils_string_create();

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, CUTILS_STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");

    free(str->_s);
    free(str);
}

static void test_api_cutils_string_create_from(void **state) {
    struct cutils_string* str = cutils_string_create_from("Test string.");

    assert_int_equal(str->size, 12);

    // assuming growth factor == 2
    // and initial capacity == 12
    // then capacity must have increased from 12 to 24
    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(str->_capacity, 24);

    assert_string_equal(str->_s, "Test string.");

    cutils_string_destroy(str);
}

static void test_api_cutils_string_destroy(void **state) {
    // at this point cutils_string_create has been already tested
    struct cutils_string* str = cutils_string_create();
    struct cutils_string* str2 = cutils_string_create_from("Test string.");

    // cmocka will check for memory leaks
    cutils_string_destroy(str);
    cutils_string_destroy(str2);
}


static void test_internal_cutils_string_create_allocate(void **state) {
    struct cutils_string *str = _cutils_string_create_allocate(14);

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);

    // assuming growth factor == 2
    // and initial capacity == 12
    // then capacity must have increased from 12 to 24
    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(str->_capacity, 24);

    cutils_string_destroy(str);
}

static void test_internal_calc_capacity(void **state) {
    assert_int_equal(_cutils_string_calc_capacity(0), 12);
    assert_int_equal(_cutils_string_calc_capacity(5), 12);
    assert_int_equal(_cutils_string_calc_capacity(10), 12);

    assert_int_equal(_cutils_string_calc_capacity(11), 24);
    assert_int_equal(_cutils_string_calc_capacity(17), 24);
    assert_int_equal(_cutils_string_calc_capacity(22), 24);

    assert_int_equal(_cutils_string_calc_capacity(23), 48);
    assert_int_equal(_cutils_string_calc_capacity(36), 48);
    assert_int_equal(_cutils_string_calc_capacity(46), 48);

    assert_int_equal(_cutils_string_calc_capacity(47), 96);
    assert_int_equal(_cutils_string_calc_capacity(75), 96);
    assert_int_equal(_cutils_string_calc_capacity(94), 96);
}

static void test_internal_cutils_string_realloc(void **state) {
    struct cutils_string* str = *state;
    int retc;

    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);

    // GROWTH
    retc = _cutils_string_realloc(str, 16);
    str->size = 16;
    assert_int_equal(retc, _REALLOC_CHANGED);
    assert_int_equal(str->_capacity, 24);

    // GROWTH WITHIN LIMIT
    retc = _cutils_string_realloc(str, 18);
    str->size = 18;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 24);

    // SHRINK WITHIN LIMIT
    retc = _cutils_string_realloc(str, 9);
    str->size = 9;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 24);

    // SHRINK
    retc = _cutils_string_realloc(str, 8);
    str->size = 8;
    assert_int_equal(retc, _REALLOC_CHANGED);
    assert_int_equal(str->_capacity, 12);

    // NO CHANGE
    retc = _cutils_string_realloc(str, 4);
    str->size = 4;
    assert_int_equal(retc, _REALLOC_NO_CHANGE);
    assert_int_equal(str->_capacity, 12);

    // finish
    *state = str;
}

static void test_api_cutils_string_copy(void **state) {
    struct cutils_string* dst = cutils_string_create_from("Test string.");

    const char* src = "A definitely new kind str.";
    unsigned int src_n = 26;

    cutils_string_copy(dst, src);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, 26);

    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2 * 2);
    assert_string_equal(dst->_s, src);

    cutils_string_destroy(dst);
}

static void test_api_cutils_string_empty(void **state) {
    struct cutils_string* str = cutils_string_create_from("Test string.");

    cutils_string_empty(str);

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, CUTILS_STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");

    cutils_string_destroy(str);
}

static void test_api_cutils_string_append_chrlst(void **state) {
    struct cutils_string* dst = cutils_string_create_from("Test string.");

    unsigned int dst_n = dst->size;

    const char* src = "A definitely new kind str.";
    unsigned int src_n = 26;

    cutils_string_append_chrlst(dst, src);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, src_n + dst_n);

    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2 * 2);
    assert_string_equal(dst->_s, "Test string.A definitely new kind str.");

    cutils_string_destroy(dst);
}

static void test_api_cutils_string_append_chr(void **state) {
    struct cutils_string* dst = cutils_string_create_from("Test string");

    const char chr = '.';

    cutils_string_append_chr(dst, chr);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, 12);

    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2);
    assert_string_equal(dst->_s, "Test string.");

    cutils_string_destroy(dst);
}

static void test_api_cutils_string_at(void **state) {
    struct cutils_string* str = cutils_string_create_from("Test string.");
    
    assert_int_equal(cutils_string_at(str, 0), 'T');
    assert_int_equal(cutils_string_at(str, 5), 's');
    assert_int_equal(cutils_string_at(str, 9), 'n');

    cutils_string_destroy(str);
}

static void test_api_cutils_string_pop(void **state) {
    struct cutils_string* str = cutils_string_create_from("Test string.");

    unsigned int before_size = str->size;
    unsigned int before_capacity = str->_capacity;

    char popped = cutils_string_pop(str);

    assert_int_equal(popped, '.');
    assert_int_equal(before_size-1, str->size);

    // assert capacity
    assert_int_equal(CUTILS_STRING_GROWTH_FACTOR, 2);
    assert_int_equal(CUTILS_STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(before_capacity, CUTILS_STRING_INITIAL_CAPACITY * CUTILS_STRING_GROWTH_FACTOR);

    cutils_string_pop(str);
    cutils_string_pop(str);
    cutils_string_pop(str);

    assert_int_equal(str->_capacity, CUTILS_STRING_INITIAL_CAPACITY);

    cutils_string_destroy(str);
}

/* -------- end of tests --------- */

static int setup_cutils_string_empty(void **state) {
    *state = cutils_string_create();
    return 0;
}

static int setup_cutils_string_teststr(void **state) {
    *state = cutils_string_create_from("Test string.");
    return 0;
}

static int teardown_cutils_string_destroy(void **state) {
    cutils_string_destroy(*state);
    return 0;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_internal_calc_capacity),
        cmocka_unit_test(test_api_cutils_string_create),
        cmocka_unit_test(test_api_cutils_string_create_from),
        cmocka_unit_test(test_api_cutils_string_destroy),
        cmocka_unit_test(test_internal_cutils_string_create_allocate),
        cmocka_unit_test_setup_teardown(test_internal_cutils_string_realloc, setup_cutils_string_empty, teardown_cutils_string_destroy),
        cmocka_unit_test(test_api_cutils_string_copy),
        cmocka_unit_test(test_api_cutils_string_empty),
        cmocka_unit_test(test_api_cutils_string_append_chrlst),
        cmocka_unit_test(test_api_cutils_string_append_chr),
        cmocka_unit_test(test_api_cutils_string_at),
        cmocka_unit_test(test_api_cutils_string_pop),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}