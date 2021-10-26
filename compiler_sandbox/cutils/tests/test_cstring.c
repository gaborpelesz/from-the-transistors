#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#define UNIT_TESTING 1

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

// void _my_free(void *ptr, const char* file, const int line) {
//     printf("FREE has been calles\n");
//     _test_free(ptr, file, line);
// }
// #define free(ptr) _my_free(void *ptr, const char* file, const int line)

#include <cutils/cstring.h>
// #include <cutils/arrayi.h>

/* A test case that does nothing and succeeds. */
static void test_string_create(void **state) {
    struct string* str = string_create();

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");

    free(str->_s);
    free(str);
}

static void test_string_destroy(void **state) {
    // at this point string_create has been already tested
    struct string* str = string_create();

    assert_non_null(str);

    string_destroy(str); // cmocka will check for memory leaks
}

static void test_string_create_from(void **state) {
    struct string* str = string_create_from("Test string.");

    assert_int_equal(str->size, 12);

    // assuming growth factor == 2
    // and initial capacity == 12
    // then capacity must have increased from 12 to 24
    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(str->_capacity, 24);

    *state = str;
}

static void test_string_create_allocate(void **state) {
    assert_true(0);
}

static void test_string_realloc_growth(void **state) {
    assert_true(0);
}

static void test_string_realloc_shrink(void **state) {
    assert_true(0);
}

static void test_string_copy(void **state) {
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
    assert_string_equal(dst->_s, "A definitely new kind str.");

    *state = dst;
}

static void test_string_empty(void **state) {
    struct string* str = string_create_from("Test string.");

    string_empty(str);

    assert_non_null(str);
    assert_non_null(str->_s);

    assert_int_equal(str->size, 0);
    assert_int_equal(str->_capacity, STRING_INITIAL_CAPACITY);
    assert_string_equal(str->_s, "");
}

static void test_string_append_chrlst(void **state) {
    struct string* dst = string_create_from("Test string.");
    const char* src = "A definitely new kind str.";
    unsigned int src_n = 26;

    string_append_chrlst(dst, src);

    assert_non_null(dst);
    assert_non_null(dst->_s);

    assert_int_equal(dst->size, 26);

    assert_int_equal(STRING_GROWTH_FACTOR, 2);
    assert_int_equal(STRING_INITIAL_CAPACITY, 12);
    assert_int_equal(dst->_capacity, 12 * 2 * 2);
    assert_string_equal(dst->_s, "A definitely new kind str.");
}

static void test_string_append_chr(void **state) {
    assert_true(0);
}

static void test_string_at(void **state) {
    assert_true(0);
}

static void test_string_pop(void **state) {
    assert_true(0);
}

static int teardown_string_destroy(void **state) {
    string_destroy(*state);
    return 0;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_string_create),
        cmocka_unit_test(test_string_destroy),
        cmocka_unit_test_teardown(test_string_create_from, teardown_string_destroy),
        cmocka_unit_test(test_string_create_allocate),
        cmocka_unit_test(test_string_realloc_growth),
        cmocka_unit_test(test_string_realloc_shrink),
        cmocka_unit_test_teardown(test_string_copy, teardown_string_destroy),
        cmocka_unit_test_teardown(test_string_empty, teardown_string_destroy),
        cmocka_unit_test_teardown(test_string_append_chrlst, teardown_string_destroy),
        cmocka_unit_test_teardown(test_string_append_chr, teardown_string_destroy),
        cmocka_unit_test_teardown(test_string_at, teardown_string_destroy),
        cmocka_unit_test_teardown(test_string_pop, teardown_string_destroy),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}