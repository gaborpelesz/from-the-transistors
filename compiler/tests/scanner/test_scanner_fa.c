#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <cutils/arrayi.h>
#include <scanner_utils/fa.h>

void print_is_accepting(unsigned int *accepting) {
    printf("{");
    for (int i = 0; i < 4; i++) {
        printf("[");
        for (int j = 0; j < 32; j++) {
            printf("%d", (accepting[i] >> j) & 1);

            if (j != 31) {
                printf(", ");
            }
        }
        printf("]");

        if (i != 3) {
            printf(", ");
        }
    }
    printf("}");
}

static void test_fa_create_destroy(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    assert_int_equal(fa->n_states, 1);
    assert_int_equal(fa->initial_state, 0);
    
    for (unsigned int i = 0; i < 4; i++) {
        assert_int_equal(fa->accepting[i], 0);
    }

    assert_int_equal(fa->n_transitions, 0);
    assert_int_equal(fa->_capacity_transitions, 10);

    assert_non_null(fa->transition);
    assert_non_null(fa->transition[0]);

    scanner_fa_destroy(fa);
}

static void test_fa_add_states_1(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    scanner_fa_add_states(fa, 1);

    assert_int_equal(fa->n_states, 2);
    assert_int_equal(fa->initial_state, 0);
    
    for (unsigned int i = 0; i < 4; i++) {
        assert_int_equal(fa->accepting[i], 0);
    }

    assert_int_equal(fa->n_transitions, 0);
    assert_int_equal(fa->_capacity_transitions, 10);

    assert_non_null(fa->transition);
    assert_non_null(fa->transition[0]);

    scanner_fa_destroy(fa);
}

static void test_fa_add_states_40(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    scanner_fa_add_states(fa, 40);

    assert_int_equal(fa->n_states, 41);
    assert_int_equal(fa->initial_state, 0);

    // from `i = 1` because the error state is never NULL
    // (it always points to the start of the char to transition array)
    for (unsigned int i = 1; i < fa->n_states; i++) {
        assert_null(fa->transition[i]);
    }
    
    for (unsigned int i = 0; i < 4; i++) {
        assert_int_equal(fa->accepting[i], 0);
    }

    assert_int_equal(fa->n_transitions, 0);
    assert_int_equal(fa->_capacity_transitions, 10);

    assert_non_null(fa->transition);
    assert_non_null(fa->transition[0]);

    scanner_fa_destroy(fa);
}

static void test_fa_set_accepting(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    scanner_fa_add_states(fa, 2);
    // set the 2nd state as accepting
    scanner_fa_set_accepting(fa, 2, 1);

    assert_int_equal(fa->n_states, 3);
    assert_int_equal(fa->initial_state, 0);

    // from `i = 1` because the error state is never NULL
    // (it always points to the start of the char to transition array)
    for (unsigned int i = 1; i < fa->n_states; i++) {
        assert_null(fa->transition[i]);
    }
    
    for (unsigned int i = 0; i < 4; i++) {
        if (i == 0) {
            assert_int_equal(fa->accepting[i], 4);
        } else {
            assert_int_equal(fa->accepting[i], 0);
        }
    }

    assert_int_equal(fa->n_transitions, 0);
    assert_int_equal(fa->_capacity_transitions, 10);

    assert_non_null(fa->transition);
    assert_non_null(fa->transition[0]);

    scanner_fa_destroy(fa);
}

static void test_fa_add_transition_simple(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    // creating the following regex: a(a|b)*
    scanner_fa_add_states(fa, 2);
    scanner_fa_add_transition(fa, 1, 'a', 2);
    scanner_fa_add_transition(fa, 2, 'a', 2);
    scanner_fa_add_transition(fa, 2, 'b', 2);

    assert_int_equal(scanner_dfa_next_state(fa, 1, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'b'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'c'), 0);
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'c'), 0);
    
    scanner_fa_destroy(fa);
}

static void test_fa_add_transition_simple_outorder(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    // creating the following regex: a(a|b)*
    scanner_fa_add_states(fa, 2);
    scanner_fa_add_transition(fa, 2, 'a', 2);
    scanner_fa_add_transition(fa, 1, 'a', 2);
    scanner_fa_add_transition(fa, 2, 'b', 2);
    
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'b'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'c'), 0);
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'c'), 0);
    
    scanner_fa_destroy(fa);
}

static void test_set_union(void **state) {
    unsigned int a[4] = {0, 5, 1, 2};
    unsigned int b[4] = {3, 1, 128, 0};

    scanner_fa_set_union(a, b);

    assert_int_equal(a[0], 3);
    assert_int_equal(a[1], 5);
    assert_int_equal(a[2], 129);
    assert_int_equal(a[3], 2);
}

static void test_set_zero(void **state) {
    unsigned int a[4] = {3, 1, 128, 0};

    scanner_fa_set_zero(a);

    assert_int_equal(a[0], 0);
    assert_int_equal(a[1], 0);
    assert_int_equal(a[2], 0);
    assert_int_equal(a[3], 0);
}

static void test_set_find_first_accepting(void **state) {
    unsigned int a[4] = {0, 128, 1, 5};

    unsigned int first_accepting = scanner_fa_set_find_first_accepting(a);

    assert_int_equal(first_accepting, 32+7);
}

static void test_fa_is_accepting(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    scanner_fa_add_states(fa, 10);
    scanner_fa_set_accepting(fa, 5, 1);

    scanner_fa_add_transition(fa, 5, 'a', 10);

    scanner_fa_set_accepting(fa, 10, 1);

    scanner_fa_add_transition(fa, 10, 'b', 5);

    for (int i = 0; i < fa->n_states; i++) {
        if (i == 5 || i == 10) {
            assert_true(scanner_fa_is_accepting(fa, i));
        } else {
            assert_false(scanner_fa_is_accepting(fa, i));
        }
    }

    scanner_fa_destroy(fa);
}

static void test_dfa_next_state(void **state) {
    assert_true(0);
}

static void test_nfa_next_state(void **state) {
    assert_true(0);
}

static void test_fa_thompson_create_char(void **state) {
    assert_true(0);
}

static void test_fa_thompson_alter(void **state) {
    assert_true(0);
}

static void test_fa_thompson_concat(void **state) {
    assert_true(0);
}

static void test_fa_thompson_close(void **state) {
    assert_true(0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fa_create_destroy),
        cmocka_unit_test(test_fa_add_states_1),
        cmocka_unit_test(test_fa_add_states_40),
        cmocka_unit_test(test_fa_set_accepting),
        cmocka_unit_test(test_fa_add_transition_simple),
        cmocka_unit_test(test_fa_add_transition_simple_outorder),
        cmocka_unit_test(test_set_union),
        cmocka_unit_test(test_set_zero),
        cmocka_unit_test(test_set_find_first_accepting),
        cmocka_unit_test(test_fa_is_accepting)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
