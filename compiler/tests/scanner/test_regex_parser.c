#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void test_fail(void **state) {
    assert_true(0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fail),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}