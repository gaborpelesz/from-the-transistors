#include "../include/scanner_utils/fa.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif

#define SCANNER_FA_TRANSITION_SCALING_FACTOR 2
#define SCANNER_FA_TRANSITION_INIT_SIZE 10

void print_state_transitions(struct scanner_fa_128* fa0) {
    for (unsigned int i = 0; i < fa0->n_transitions; i++) {
        printf("%d.: %ld = %c (%u), %d\n", i, (long)(fa0->transition[0]+i), (fa0->transition[0]+i)->c, (unsigned int)((fa0->transition[0]+i)->c), (fa0->transition[0]+i)->next_state);
    }
}

void print_transitions(struct scanner_fa_128* fa0) {
    for (unsigned int i = 0; i < fa0->n_states; i++) {
        printf("%ld", (long)(fa0->transition[i]));

        if (i != fa0->n_states - 1) {
            printf(", ");
        } else {
            printf("\n");
        }
    }
}

struct scanner_fa_128 *scanner_fa_create() {
    struct scanner_fa_128 *fa = malloc(sizeof(struct scanner_fa_128));
    fa->n_states = 1;
    fa->initial_state = 0;

    fa->accepting = cutils_set128_empty();

    fa->transition = calloc(sizeof(struct _scanner_fa_transition*), 1);
    fa->transition[0] = malloc(sizeof(struct _scanner_fa_transition) * SCANNER_FA_TRANSITION_INIT_SIZE);
    fa->_capacity_transitions = SCANNER_FA_TRANSITION_INIT_SIZE;
    fa->n_transitions = 0;

    return fa;
}

void scanner_fa_destroy(struct scanner_fa_128 *fa) {
    if (fa != NULL) {
        if (fa->transition != NULL) {
            if (fa->transition[0] != NULL) {
                free(fa->transition[0]);
            }
            free(fa->transition);
        }
        free(fa);
    }
}

void _scanner_fa_transition_realloc(struct scanner_fa_128 * const fa, unsigned int new_capacity) {
    struct _scanner_fa_transition *old_ptr = fa->transition[0];
    fa->transition[0] = realloc(fa->transition[0], new_capacity * sizeof(struct _scanner_fa_transition));

    if (fa->transition[0] == NULL) {
        printf("ERROR: `realloc` failed when reallocating transitions to FA.\n");
        exit(EXIT_FAILURE);
    }

    fa->_capacity_transitions = new_capacity;

    if (old_ptr != fa->transition[0]) {
        // preserving correct pointers in the state pointer array
        for (unsigned int i = 1; i < fa->n_states; i++) {
            if (fa->transition[i] != NULL) {
                fa->transition[i] = (struct _scanner_fa_transition*)((unsigned long)(fa->transition[0]) + ((unsigned long)(fa->transition[i]) - (unsigned long)old_ptr));
            }
        }
    }
}

void scanner_fa_add_states(struct scanner_fa_128 *fa, unsigned char n_states_to_add) {
    if (n_states_to_add == 0) {
        return;
    }

    unsigned char n_states_new = fa->n_states + n_states_to_add;
    struct _scanner_fa_transition **new_ptr = realloc(fa->transition, n_states_new * sizeof(struct _scanner_fa_transition *));

    if (new_ptr != NULL) {
        fa->transition = new_ptr;
    } else {
        printf("ERROR: `realloc` failed when adding states to FA.\n");
        scanner_fa_destroy(fa);
        exit(EXIT_FAILURE);
    }

    for (unsigned char i = fa->n_states; i < n_states_new; i++) {
        fa->transition[i] = NULL;
    }

    fa->n_states = n_states_new;
}

/**
 * Right shifts by one the transition array of an FA from a specified index
 * 
 * @param from_element_i: the first index to be right shifted
 */
void _fa_trans_right_shift(struct scanner_fa_128 *fa, unsigned int from_element_i) {
    for (unsigned int i = fa->n_transitions - 1; i >= from_element_i; i--) {
        fa->transition[0][i] = fa->transition[0][i-1];
    }
}

void _fa_state_right_shift(struct scanner_fa_128 *fa, unsigned int from_state) {
    for (unsigned int i = from_state; i < fa->n_states; i++) {
        if (fa->transition[i] != NULL) {
            fa->transition[i]++; // pointer++ so it points to the next address
        }
    }
}

/**
 * Finds the first non-null state to the right of the current state.
 * If the current state is the last non-null state, this function returns `fa->n_states`.
 */
unsigned char _fa_find_closest_right(const struct scanner_fa_128 * const fa, unsigned char state) {
    unsigned char closest_right_i;
    for (closest_right_i = state+1; closest_right_i < fa->n_states && fa->transition[closest_right_i] == NULL; ++closest_right_i);
    return closest_right_i;
}

void scanner_fa_add_transition(struct scanner_fa_128 * const fa, unsigned char state, unsigned char character, unsigned char next_state) {
    if (state == 0) {
        printf("WARNING: trying to add transition to the error-state. The attempt didn't have any consequences because you can't add transitions to the error state.\n");
        return;
    }

    if (next_state == 0) {
        printf("WARNING: adding transition from character to an error state is not necessary. Every undefined character transition will result in a transition to the error state.\n");
        return;
    }

    if (scanner_dfa_next_state(fa, state, character) != 0) {
        printf("WARNING: adding already existing transition to state `%d` with char `%c`. This finite automaton will behave as an non-deterministic FA from this point.\n", state, character);
    }

    // realloc if needed to give space for the new element
    fa->n_transitions++;
    if (fa->n_transitions >= fa->_capacity_transitions) {
        _scanner_fa_transition_realloc(fa, fa->_capacity_transitions * SCANNER_FA_TRANSITION_SCALING_FACTOR);
    }

    // closest non-null member on the right
    unsigned char closest_right_i = _fa_find_closest_right(fa, state);

    struct _scanner_fa_transition *inserted_elem;

    // if there was no non-null state pointer to the right
    // then insert an element to the end of the character transition array
    if (closest_right_i == fa->n_states) {
        inserted_elem = fa->transition[0] + (fa->n_transitions - 1);
    }
    // else shift everything from the closest element to the right by 1
    // and insert an element just before the first element of the closest state
    else {
        // TODO: calculation of from_element_i has issues. Pointer substraction with turning both of them into unsigned long won't return correct answer
        //       because the size of an element is 2
        unsigned int from_element_i = (unsigned long)(fa->transition[closest_right_i]) - (unsigned long)(fa->transition[0]); // ">> 1" is an integer divison by 2
        from_element_i = from_element_i / sizeof(struct _scanner_fa_transition) + 1;
        _fa_trans_right_shift(fa, from_element_i);
        _fa_state_right_shift(fa, closest_right_i);

        inserted_elem = fa->transition[closest_right_i] - 1; // pointer to point one element to the left
    }

    inserted_elem->c = character;
    inserted_elem->next_state = next_state;

    // if this transition was the first transition for the state
    if (fa->transition[state] == NULL) {
        fa->transition[state] = inserted_elem;
    }
}

void scanner_fa_set_accepting(struct scanner_fa_128 * const fa, unsigned char state, unsigned char accepting) {
    if (state > fa->n_states) {
        printf("ERROR: scanner -> trying to set %d. state to accepting but FA only has %d of states.\n", state, fa->n_states);
        exit(EXIT_FAILURE);
    }

    struct cutils_set128 A = cutils_set128_create(state);

    if (accepting) {
        fa->accepting = cutils_set128_union(fa->accepting, A);
    } else {
        A = cutils_set128_negate(A);
        fa->accepting = cutils_set128_intersection(fa->accepting, A); 
    }
}

unsigned char scanner_fa_is_accepting(const struct scanner_fa_128 * const fa, unsigned short state) {
    if (state > fa->n_states) {
        printf("ERROR: scanner -> trying to set %d. state to accepting but FA only has %d of states.\n", state, fa->n_states);
        exit(EXIT_FAILURE);
    }

    return cutils_set128_has_element(fa->accepting, state);
}

void scanner_fa_set_union(unsigned int *a, const unsigned int * const b) {
    for (unsigned int i = 0; i < 4; i++) {
        a[i] = a[i] | b[i];
    }
}

void scanner_fa_set_zero(unsigned int *a) {
    for (unsigned int i = 0; i < 4; i++) {
        a[i] = 0;
    }
}

struct _scanner_fa_transition * _fa_find_closest_right_ptr(const struct scanner_fa_128 * const fa, unsigned char state) {
    unsigned char closest_right_i = _fa_find_closest_right(fa, state);

    struct _scanner_fa_transition *end;

    // if `state` is the last state
    if (closest_right_i == fa->n_states) {
        end = fa->transition[0] + fa->n_transitions;
    } else {
        end = fa->transition[closest_right_i];
    }

    return end;
}

unsigned char scanner_dfa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch) {
    if (state >= fa->n_states) {
        printf("ERROR: Trying to find next state for a state (%d) that does not exist. (n_states=%d)\n", state, fa->n_states);
        exit(EXIT_FAILURE);
    }

    if (state == 0 || fa->transition[state] == NULL) {
        return 0;
    }

    struct _scanner_fa_transition *end = _fa_find_closest_right_ptr(fa, state);

    for (struct _scanner_fa_transition *i_ptr = fa->transition[state]; i_ptr != end; i_ptr++) {
        if (i_ptr->c == ch)
            return i_ptr->next_state;
    }

    return 0;
}

void scanner_nfa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch, struct cutils_arrayi ** next_states) {
    if (*next_states == NULL) {
        *next_states = cutils_arrayi_create();
    } else {
        cutils_arrayi_empty(*next_states);
    }

    if (state == 0) {
        return;
    }


    struct _scanner_fa_transition *end = _fa_find_closest_right_ptr(fa, state);

    for (struct _scanner_fa_transition *i_ptr = fa->transition[state]; i_ptr != end; i_ptr++) {
        if (i_ptr->c == ch) {
            cutils_arrayi_push(*next_states, i_ptr->next_state);
        }
    }
}

struct scanner_fa_128 *scanner_fa_thompson_create_char(unsigned char character) {
    struct scanner_fa_128 *fa = scanner_fa_create();
    scanner_fa_add_states(fa, 2);
    scanner_fa_set_accepting(fa, 2, 1);
    fa->initial_state = 1;
    scanner_fa_add_transition(fa, 1, character, 2);
    return fa;
}

void scanner_fa_merge(struct scanner_fa_128 * const fa0, const struct scanner_fa_128 * const fa1) {
    if (fa0->n_states + fa1->n_states - 1 > 128) {
        printf("ERROR: scanner_fa_merge -> resulting FA is too big. Aborting...\n");
        exit(EXIT_FAILURE);
    }

    int fa0_n = fa0->n_states;
    int fa0_n_transitions = fa0->n_transitions;

    scanner_fa_add_states(fa0, fa1->n_states - 1);

    if (fa0->n_transitions + fa1->n_transitions > fa0->_capacity_transitions) {
        unsigned int fa0_new_capacity;

        #if SCANNER_FA_TRANSITION_SCALING_FACTOR == 2
            fa0_new_capacity = SCANNER_FA_TRANSITION_INIT_SIZE << (int)ceil(log2((fa0->n_transitions + fa1->n_transitions) / 10.0)); 
        #else
            int scale_factor = SCANNER_FA_TRANSITION_SCALING_FACTOR;
            int init = SCANNER_FA_TRANSITION_INIT_SIZE;
            int t_sum = fa0->n_transitions + fa1->n_transitions;
            int scale = ceil(log10((t_sum)/10.0)/log10(scale_factor));
            fa0_new_capacity = init * pow(scale_factor, scale);
        #endif

        _scanner_fa_transition_realloc(fa0, fa0_new_capacity);
    }

    // copying char to next state array
    memcpy(fa0->transition[0]+fa0->n_transitions, fa1->transition[0], fa1->n_transitions * sizeof(struct _scanner_fa_transition));
    fa0->n_transitions += fa1->n_transitions;

    // copying state transition array pointers values
    struct _scanner_fa_transition *fa1_transitions_start = fa0->transition[0] + fa0_n_transitions;
    for (unsigned int i = 1; i < fa1->n_states; i++) {
        if (fa1->transition[i] != NULL) {
            fa0->transition[fa0_n - 1 + i] = fa1_transitions_start + (fa1->transition[i] - fa1->transition[0]);
        }
    }

    // changing the next states for the copied next states
    for (unsigned int i = 0; i < fa1->n_transitions; i++) {
        unsigned int j = fa0_n_transitions + i;
        (fa0->transition[0]+j)->next_state += fa0_n-1;
    }

    for (int i = 1; i < fa1->n_states; i++) {
        scanner_fa_set_accepting(fa0, i+fa0_n-1, scanner_fa_is_accepting(fa1, i));
    }
}

void scanner_fa_thompson_concat(struct scanner_fa_128 * const fa0, const struct scanner_fa_128 * const fa1) {
    int fa0_end_state = cutils_set128_smallest(fa0->accepting);
    int fa1_end_state = cutils_set128_smallest(fa1->accepting);
    int fa0_n_states = fa0->n_states;

    // setting `fa1` as the only accepting state after concatenation
    // the resulting accepting state will be the only accepting state of `fa1`
    // if the FAs were not constructed with `scanner_fa_thompson` functions then
    // it is possible that there will be multiple accepting states.
    // The latter is the user's responsibility!
    scanner_fa_set_accepting(fa0, fa0_end_state, 0);

    scanner_fa_merge(fa0, fa1);

    // adding concat empty transition 
    // `fa0_n_states` is the first state of `fa1` after the merge
    scanner_fa_add_transition(fa0, fa0_end_state, 0x00, fa1->initial_state + fa0_n_states - 1);
}

void scanner_fa_thompson_alter(struct scanner_fa_128 * const fa0, const struct scanner_fa_128 * const fa1) {
    int fa0_initial = fa0->initial_state;
    int fa0_end = cutils_set128_smallest(fa0->accepting);

    int fa1_initial = fa1->initial_state + fa0->n_states - 1;
    int fa1_end = cutils_set128_smallest(fa1->accepting) + fa0->n_states - 1;

    scanner_fa_merge(fa0, fa1);

    // no accepting state remains accepting
    fa0->accepting = cutils_set128_empty();

    // creating initial state and final state
    scanner_fa_add_states(fa0, 2);
    fa0->initial_state = fa0->n_states - 2;
    scanner_fa_set_accepting(fa0, fa0->n_states - 1, 1);

    // initial state's empty transition to both alternatives
    scanner_fa_add_transition(fa0, fa0->initial_state, 0x00, fa0_initial);
    scanner_fa_add_transition(fa0, fa0->initial_state, 0x00, fa1_initial);

    // both alternatives' end state empty transition to final state
    scanner_fa_add_transition(fa0, fa0_end, 0x00, fa0->n_states - 1);
    scanner_fa_add_transition(fa0, fa1_end, 0x00, fa0->n_states - 1);
}

void scanner_fa_thompson_close(struct scanner_fa_128 * const fa) {
    int fa_initial = fa->initial_state;
    int fa_end = cutils_set128_smallest(fa->accepting);

    fa->accepting = cutils_set128_empty(fa->accepting);

    scanner_fa_add_states(fa, 2);
    fa->initial_state = fa->n_states - 2;
    scanner_fa_set_accepting(fa, fa->n_states - 1, 1);

    scanner_fa_add_transition(fa, fa->initial_state, 0x00, fa_initial);
    scanner_fa_add_transition(fa, fa->initial_state, 0x00, fa->n_states - 1);
    scanner_fa_add_transition(fa, fa_end, 0x00, fa_initial);
    scanner_fa_add_transition(fa, fa_end, 0x00, fa->n_states - 1);
}