#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cutils/arrayi.h>
#include <scanner_utils/fa.h>

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

    assert_int_equal(fa->n_states, 3);
    assert_int_equal(fa->initial_state, 0);


    for (unsigned int i = 0; i < 4; i++) {
        assert_int_equal(fa->accepting[i], 0);
    }

    assert_int_equal(fa->n_transitions, 3);
    assert_int_equal(fa->_capacity_transitions, 10);

    assert_non_null(fa->transition);
    assert_non_null(fa->transition[0]);

    // from `i = 1` because the error state is never NULL
    // (it always points to the start of the char to transition array)
    for (unsigned int i = 1; i < fa->n_states; i++) {
        if (i == 1 || i == 2) {
            assert_non_null(fa->transition[i]);
        } else {
            assert_null(fa->transition[i]);
        }
    }

    // TODO, something is not right at how we store the transitions with scanner_fa_add_transition
    printf("hello start\n");
    printf("%u, %u, %u\n", fa->transition[0], fa->transition[1], fa->transition[2]);
    printf("%c, %c, %c\n", fa->transition[0]->c, fa->transition[1]->c, fa->transition[2]->c);

    assert_int_equal(scanner_dfa_next_state(fa, 1, 'a'), 2);
    printf("hello end\n");
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'b'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'c'), 0);
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'c'), 0);
    
    scanner_fa_destroy(fa);
}

static void test_set_union(void **state) {

}

static void test_set_zero(void **state) {

}

static void test_set_find_first_accepting(void **state) {

}

static void test_fa_is_accepting(void **state) {

}

static void test_dfa_next_state(void **state) {

}

static void test_nfa_next_state(void **state) {

}

static void test_fa_thompson_create_char(void **state) {

}

static void test_fa_thompson_alter(void **state) {

}

static void test_fa_thompson_concat(void **state) {

}

static void test_fa_thompson_close(void **state) {

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fa_create_destroy),
        cmocka_unit_test(test_fa_add_states_1),
        cmocka_unit_test(test_fa_add_states_40),
        cmocka_unit_test(test_fa_set_accepting),
        cmocka_unit_test(test_fa_add_transition_simple),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
