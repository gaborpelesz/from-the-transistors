#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include <cutils/set.h>
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
    
    assert_true(cutils_set128_isempty(fa->accepting));

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
    
    assert_true(cutils_set128_isempty(fa->accepting));

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
    
    assert_true(cutils_set128_isempty(fa->accepting));

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
    
    assert_int_equal(cutils_set128_size(fa->accepting), 1);
    assert_true(cutils_set128_has_element(fa->accepting, 2));

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
    struct scanner_fa_128 *fa = scanner_fa_create();

    // creating the following regex: a(a|b)*
    scanner_fa_add_states(fa, 2);
    scanner_fa_add_transition(fa, 1, 'a', 2);
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'a'), 2);

    scanner_fa_add_transition(fa, 2, 'a', 2);
    scanner_fa_add_transition(fa, 2, 'b', 2);

    assert_int_equal(scanner_dfa_next_state(fa, 2, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'b'), 2);
    assert_int_equal(scanner_dfa_next_state(fa, 2, 'c'), 0);
    assert_int_equal(scanner_dfa_next_state(fa, 1, 'c'), 0);

    scanner_fa_destroy(fa);
}

static void test_nfa_next_state(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_create();

    // creating the following regex: a(a|b)*
    scanner_fa_add_states(fa, 3);

    scanner_fa_add_transition(fa, 1, 'a', 2);
    scanner_fa_add_transition(fa, 1, 'a', 3);
    scanner_fa_add_transition(fa, 2, 'b', 2);
    scanner_fa_add_transition(fa, 2, 'b', 1);

    struct cutils_arrayi* arr = NULL;
    scanner_nfa_next_state(fa, 1, 'a', &arr);

    assert_int_equal(arr->size, 2);
    assert_int_equal(cutils_arrayi_at(arr, 0), 2);
    assert_int_equal(cutils_arrayi_at(arr, 1), 3);

    scanner_nfa_next_state(fa, 2, 'b', &arr);

    assert_int_equal(arr->size, 2);
    assert_int_equal(cutils_arrayi_at(arr, 0), 2);
    assert_int_equal(cutils_arrayi_at(arr, 1), 1);

    cutils_arrayi_destroy(arr);
    scanner_fa_destroy(fa);
}

static void test_fa_merge(void **state) {
    // creating the following regex: a(a|b)*
    struct scanner_fa_128 *fa0 = scanner_fa_create();
    scanner_fa_add_states(fa0, 2);
    scanner_fa_add_transition(fa0, 1, 'a', 2);
    scanner_fa_add_transition(fa0, 2, 'a', 2);
    scanner_fa_add_transition(fa0, 2, 'b', 2);
    scanner_fa_set_accepting(fa0, 2, 1);
    fa0->initial_state = 1;

    // creating the following regex: c(c|d)*
    struct scanner_fa_128 *fa1 = scanner_fa_create();
    scanner_fa_add_states(fa1, 2);
    scanner_fa_add_transition(fa1, 1, 'c', 2);
    scanner_fa_add_transition(fa1, 2, 'c', 2);
    scanner_fa_add_transition(fa1, 2, 'd', 2);
    scanner_fa_set_accepting(fa1, 2, 1);
    fa1->initial_state = 1;

    scanner_fa_merge(fa0, fa1);

    assert_int_equal(fa0->initial_state, 1);
    assert_int_equal(fa0->n_states, 5);
    assert_int_equal(fa0->n_transitions, 6);

    assert_int_equal(cutils_set128_size(fa0->accepting), 2);
    assert_true(cutils_set128_has_element(fa0->accepting, 2));
    assert_true(cutils_set128_has_element(fa0->accepting, 4));

    assert_int_equal(scanner_dfa_next_state(fa0, 1, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa0, 2, 'a'), 2);
    assert_int_equal(scanner_dfa_next_state(fa0, 2, 'b'), 2);

    assert_int_equal(scanner_dfa_next_state(fa0, 3, 'c'), 4);
    assert_int_equal(scanner_dfa_next_state(fa0, 4, 'c'), 4);
    assert_int_equal(scanner_dfa_next_state(fa0, 4, 'd'), 4);

    scanner_fa_destroy(fa0);
    scanner_fa_destroy(fa1);
}

static void test_fa_thompson_create_char(void **state) {
    struct scanner_fa_128 *fa = scanner_fa_thompson_create_char('a');

    assert_int_equal(scanner_dfa_next_state(fa, 1, 'a'), 2);
    assert_int_equal(cutils_set128_size(fa->accepting), 1);
    assert_true(cutils_set128_has_element(fa->accepting, 2));
    assert_int_equal(fa->initial_state, 1);
    assert_int_equal(fa->n_states, 3);
    assert_int_equal(fa->n_transitions, 1);

    scanner_fa_destroy(fa);
}

static void test_fa_thompson_alter(void **state) {
    // Thompson create: a|b
    struct scanner_fa_128 *fa0 = scanner_fa_thompson_create_char('a');
    struct scanner_fa_128 *fa1 = scanner_fa_thompson_create_char('b');
    struct cutils_arrayi *next_states = NULL;

    scanner_fa_thompson_alter(fa0, fa1);

    // Test replicates the example from book "Engineering a compiler"
    // page 46, Figure 2.4 (d) NFA for "a | b"
    assert_int_equal(fa0->initial_state, 5);
    assert_int_equal(fa0->n_states, 7);
    assert_int_equal(fa0->n_transitions, 6);
    for (unsigned int i = 0; i < 7; i++) {
        assert_int_equal(scanner_fa_is_accepting(fa0, i), i==6);
    }

    // s_m epsilon transitions to s_i and s_k
    scanner_nfa_next_state(fa0, 5, 0x00, &next_states);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 1);
    assert_int_equal(cutils_arrayi_at(next_states, 1), 3);
    // s_i transition to s_j
    scanner_nfa_next_state(fa0, 1, 'a', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 2);
    // s_k transition to s_l
    scanner_nfa_next_state(fa0, 3, 'b', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 4);
    // s_j epsilon transition to s_n
    scanner_nfa_next_state(fa0, 2, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 6);
    // s_l epsilon transition to s_n
    scanner_nfa_next_state(fa0, 4, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 6);

    scanner_fa_destroy(fa0);
    scanner_fa_destroy(fa1);
    cutils_arrayi_destroy(next_states);
}

static void test_fa_thompson_concat(void **state) {
    // Thompson create: ab
    struct scanner_fa_128 *fa0 = scanner_fa_thompson_create_char('a');
    struct scanner_fa_128 *fa1 = scanner_fa_thompson_create_char('b');
    struct cutils_arrayi *next_states = NULL;

    scanner_fa_thompson_concat(fa0, fa1);
    
    // Test replicates the example from book "Engineering a compiler"
    // page 46, Figure 2.4 (c) NFA for "ab"
    assert_int_equal(fa0->initial_state, 1);
    assert_int_equal(fa0->n_states, 5);
    assert_int_equal(fa0->n_transitions, 3);
    for (unsigned int i = 0; i < 5; i++) {
        assert_int_equal(scanner_fa_is_accepting(fa0, i), i==4);
    }

    // s_i transition to s_j
    scanner_nfa_next_state(fa0, 1, 'a', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 2);

    // s_j epsilon transition to s_k
    scanner_nfa_next_state(fa0, 2, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 3);

    // s_k transition s_l
    scanner_nfa_next_state(fa0, 3, 'b', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 4);

    scanner_fa_destroy(fa0);
    scanner_fa_destroy(fa1);
    cutils_arrayi_destroy(next_states);
}

static void test_fa_thompson_close(void **state) {
    // Thompson create: a*
    struct scanner_fa_128 *fa0 = scanner_fa_thompson_create_char('a');
    struct cutils_arrayi *next_states = NULL;

    scanner_fa_thompson_close(fa0);
    
    // Test replicates the example from book "Engineering a compiler"
    // page 46, Figure 2.4 (e) NFA for "a*"
    assert_int_equal(fa0->initial_state, 3);
    assert_int_equal(fa0->n_states, 5);
    assert_int_equal(fa0->n_transitions, 5);
    for (unsigned int i = 0; i < 5; i++) {
        assert_int_equal(scanner_fa_is_accepting(fa0, i), i==4);
    }

    // s_i transition to s_j
    scanner_nfa_next_state(fa0, 1, 'a', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 2);

    // s_j epsilon transitions to s_p and s_q
    scanner_nfa_next_state(fa0, 2, 0x00, &next_states);
    assert_int_equal(next_states->size, 2);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 1);
    assert_int_equal(cutils_arrayi_at(next_states, 1), 4);

    // s_p epsilon transitions to s_i and s_q
    scanner_nfa_next_state(fa0, 3, 0x00, &next_states);
    assert_int_equal(next_states->size, 2);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 1);
    assert_int_equal(cutils_arrayi_at(next_states, 1), 4);

    scanner_fa_destroy(fa0);
    cutils_arrayi_destroy(next_states);
}

static void test_fa_thompson_all(void **state) {
    // building a(b|c)* with Thompson construction
    // Test replicates the example from book "Engineering a compiler"
    // page 47, Figure 2.5
    struct scanner_fa_128 *fa0 = scanner_fa_thompson_create_char('a');
    struct scanner_fa_128 *fa1 = scanner_fa_thompson_create_char('b');
    struct scanner_fa_128 *fa2 = scanner_fa_thompson_create_char('c');
    struct cutils_arrayi *next_states = NULL;

    scanner_fa_thompson_alter(fa1, fa2);
    scanner_fa_thompson_close(fa1);
    scanner_fa_thompson_concat(fa0, fa1);

    assert_int_equal(fa0->initial_state, 1);
    assert_int_equal(fa0->n_states, 11);
    assert_int_equal(fa0->n_transitions, 12);
    for (unsigned int i = 0; i < fa0->n_states; i++) {
        assert_int_equal(scanner_fa_is_accepting(fa0, i), i==10);
    }
    
    // s0 -> s1
    scanner_nfa_next_state(fa0, 1, 'a', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 2);
    
    // s1 -> s8
    scanner_nfa_next_state(fa0, 2, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 9);

    // s8 -> s6, s9
    scanner_nfa_next_state(fa0, 9, 0x00, &next_states);
    assert_int_equal(next_states->size, 2);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 7);
    assert_int_equal(cutils_arrayi_at(next_states, 1), 10);
    
    // s6 -> s2, s4
    scanner_nfa_next_state(fa0, 7, 0x00, &next_states);
    assert_int_equal(next_states->size, 2);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 3);
    assert_int_equal(cutils_arrayi_at(next_states, 1), 5);

    // s2 -> s3
    scanner_nfa_next_state(fa0, 3, 'b', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 4);
    
    // s4 -> s5
    scanner_nfa_next_state(fa0, 5, 'c', &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 6);

    // s3 -> s7
    scanner_nfa_next_state(fa0, 4, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 8);

    // s5 -> s7
    scanner_nfa_next_state(fa0, 6, 0x00, &next_states);
    assert_int_equal(next_states->size, 1);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 8); 

    // s7 -> s6, s9
    scanner_nfa_next_state(fa0, 8, 0x00, &next_states);
    assert_int_equal(next_states->size, 2);
    assert_int_equal(cutils_arrayi_at(next_states, 0), 7); 
    assert_int_equal(cutils_arrayi_at(next_states, 1), 10); 

    scanner_fa_destroy(fa0);
    scanner_fa_destroy(fa1);
    scanner_fa_destroy(fa2);
    cutils_arrayi_destroy(next_states);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_fa_create_destroy),
        cmocka_unit_test(test_fa_add_states_1),
        cmocka_unit_test(test_fa_add_states_40),
        cmocka_unit_test(test_fa_set_accepting),
        cmocka_unit_test(test_fa_add_transition_simple),
        cmocka_unit_test(test_fa_add_transition_simple_outorder),
        cmocka_unit_test(test_fa_is_accepting),
        cmocka_unit_test(test_dfa_next_state),
        cmocka_unit_test(test_nfa_next_state),
        cmocka_unit_test(test_fa_merge),
        cmocka_unit_test(test_fa_thompson_create_char),
        cmocka_unit_test(test_fa_thompson_alter),
        cmocka_unit_test(test_fa_thompson_concat),
        cmocka_unit_test(test_fa_thompson_close),
        cmocka_unit_test(test_fa_thompson_all)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
