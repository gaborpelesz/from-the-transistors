#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/arrayi.h>

static void test_cutils_arrayi_create(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    assert_non_null(arr);
    assert_non_null(arr->_arr);

    assert_int_equal(arr->size, 0);
    assert_int_equal(arr->_capacity, CUTILS_ARRAYI_INITIAL_CAPACITY);

    free(arr->_arr);
    free(arr);
}

static void test_cutils_arrayi_destroy(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    cutils_arrayi_destroy(arr);
}

static void test_cutils_arrayi_push(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    int list_n = 4;
    int list[] = {2, 4, 2, 3};

    for (int i = 0; i < list_n; i++) {
        cutils_arrayi_push(arr, list[i]);
    }

    assert_int_equal(arr->size, list_n);

    for (int i = 0; i < list_n; i++) {
        assert_int_equal(arr->_arr[i], list[i]);
    }

    cutils_arrayi_destroy(arr);
}

static void test_cutils_arrayi_empty(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    cutils_arrayi_push(arr, 10);
    cutils_arrayi_push(arr, 10);
    cutils_arrayi_push(arr, 10);
    cutils_arrayi_push(arr, 10);

    assert_int_equal(arr->size, 4);

    cutils_arrayi_empty(arr);

    assert_int_equal(arr->size, 0);

    cutils_arrayi_destroy(arr);
}

static void test_cutils_arrayi_pop(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    cutils_arrayi_push(arr, 15);
    cutils_arrayi_push(arr, 3);
    cutils_arrayi_push(arr, 5);
    cutils_arrayi_push(arr, 34);

    assert_int_equal(arr->size, 4);

    int popped = cutils_arrayi_pop(arr);

    assert_int_equal(arr->size, 3);
    assert_int_equal(popped, 34);

    cutils_arrayi_destroy(arr);
}

static void test_cutils_arrayi_at(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    cutils_arrayi_push(arr, 15);
    cutils_arrayi_push(arr, 3);
    cutils_arrayi_push(arr, 5);
    cutils_arrayi_push(arr, 34);

    assert_int_equal(arr->size, 4);

    int returned = cutils_arrayi_at(arr, 2);

    assert_int_equal(arr->size, 4);
    assert_int_equal(returned, 5);

    cutils_arrayi_destroy(arr);
}

static void test_cutils_arrayi_remove_at(void **state) {
    struct cutils_arrayi* arr = cutils_arrayi_create();

    cutils_arrayi_push(arr, 15);
    cutils_arrayi_push(arr, 3);
    cutils_arrayi_push(arr, 5);
    cutils_arrayi_push(arr, 34);

    assert_int_equal(arr->size, 4);

    cutils_arrayi_remove_at(arr, 2);

    assert_int_equal(arr->size, 3);
    assert_int_equal(arr->_arr[0], 15);
    assert_int_equal(arr->_arr[1], 3);
    assert_int_equal(arr->_arr[2], 34);

    cutils_arrayi_remove_at(arr, 0);

    assert_int_equal(arr->size, 2);
    assert_int_equal(arr->_arr[0], 3);
    assert_int_equal(arr->_arr[1], 34);

    cutils_arrayi_destroy(arr);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cutils_arrayi_create),
        cmocka_unit_test(test_cutils_arrayi_destroy),
        cmocka_unit_test(test_cutils_arrayi_push),
        cmocka_unit_test(test_cutils_arrayi_empty),
        cmocka_unit_test(test_cutils_arrayi_pop),
        cmocka_unit_test(test_cutils_arrayi_at),
        cmocka_unit_test(test_cutils_arrayi_remove_at),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}