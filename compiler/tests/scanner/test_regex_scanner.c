#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <scanner_utils/regex_scanner.h>

static void test_scanner_pass(void** state) {
    printf("Hello from tests\n");

    assert_int_equal(1, 1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scanner_pass),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}