#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/arrayi.h>
#include <scanner_utils/fa.h>

static void test_scanner_fa_create(void **state) {
    
}

static void test_1(void **state) {

}

static void test_2(void **state) {

}


static void test_3(void **state) {

}

static void test_fail(void **state) {
    assert_true(0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fail),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
