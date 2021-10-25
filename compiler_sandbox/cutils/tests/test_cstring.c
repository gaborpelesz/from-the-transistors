#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void null_test_failure(void **state) {
    (void) state; /* unused */
    assert_true(0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(null_test_failure),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}