#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include <cutils/set.h>

static void test_cutils_set_create(void **state) {
    const struct cutils_set128 A = cutils_set128_create(5);
    const struct cutils_set128 B = cutils_set128_create(83);

    assert_int_equal(A._bitvector[0], 1 << 5);
    assert_int_equal(A._bitvector[1], 0);
    assert_int_equal(A._bitvector[2], 0);
    assert_int_equal(A._bitvector[3], 0);

    assert_int_equal(B._bitvector[0], 0);
    assert_int_equal(B._bitvector[1], 0);
    assert_int_equal(B._bitvector[2], 1 << (83 - 64));
    assert_int_equal(B._bitvector[3], 0);
}

static void test_cutils_set_create_fromlist(void **state) {
    // #define TEST_SET_LIST_N 5
    // const char list1[TEST_SET_LIST_N] = {1, 6, 7, 9, 53};
    // const struct cutils_set128 A = cutils_set128_create_fromlist(list1, TEST_SET_LIST_N);

    // const char list2[TEST_SET_LIST_N] = {1, 6, 36, 52, 114};
    // const struct cutils_set128 B = cutils_set128_create_fromlist(list2, TEST_SET_LIST_N);

    // #define TEST_SET_LIST3_N 8
    // const char list3[TEST_SET_LIST3_N] = {0, 31, 32, 63, 64, 95, 96, 127};
    // const struct cutils_set128 C = cutils_set128_create_fromlist(list3, TEST_SET_LIST3_N);

    // assert_int_equal(A._bitvector[0], () + () + () + ());
    // assert_int_equal(A._bitvector[1], 1 << (53 - 32));
    // assert_int_equal(A._bitvector[2], 0);
    // assert_int_equal(A._bitvector[3], 0);

    // assert_int_equal(B._bitvector[0], 0);
    // assert_int_equal(B._bitvector[1], 0);
    // assert_int_equal(B._bitvector[2], 0);
    // assert_int_equal(B._bitvector[3], 0);

    // assert_int_equal(C._bitvector[0], 0);
    // assert_int_equal(C._bitvector[1], 0);
    // assert_int_equal(C._bitvector[2], 0);
    // assert_int_equal(C._bitvector[3], 0);
}

static void test_cutils_set_union(void **state) {
    struct cutils_set128 A = {{0, 5, 1, 2}};
    struct cutils_set128 B = {{3, 1, 128, 0}};

    A = cutils_set128_union(A, B);

    assert_int_equal(A._bitvector[0], 3);
    assert_int_equal(A._bitvector[1], 5);
    assert_int_equal(A._bitvector[2], 129);
    assert_int_equal(A._bitvector[3], 2);
}

static void test_cutils_set_empty(void **state) {
    struct cutils_set128 A = cutils_set128_empty();

    assert_int_equal(A._bitvector[0], 0);
    assert_int_equal(A._bitvector[1], 0);
    assert_int_equal(A._bitvector[2], 0);
    assert_int_equal(A._bitvector[3], 0);
}

static void test_cutils_set_smallest(void **state) {
    struct cutils_set128 A = {{0, 128, 1, 5}};

    unsigned int smallest = cutils_set128_smallest(A);

    assert_int_equal(smallest, 32+7);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cutils_set_create),
        cmocka_unit_test(test_cutils_set_union),
        cmocka_unit_test(test_cutils_set_empty),
        cmocka_unit_test(test_cutils_set_smallest),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}