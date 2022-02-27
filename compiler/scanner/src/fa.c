#include "../include/scanner_utils/fa.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SCANNER_FA_TRANSITION_SCALING_FACTOR 2
#define SCANNER_FA_TRANSITION_INIT_SIZE 10

struct scanner_fa_128 *scanner_fa_create() {
    struct scanner_fa_128 *fa = malloc(sizeof(struct scanner_fa_128));
    fa->n_states = 1;
    fa->initial_state = 0;

    for (unsigned int i = 0; i < 4; i++) {
        fa->accepting[i] = 0;
    }

    fa->transition = calloc(sizeof(struct _scanner_fa_transition*), 1);
    fa->transition[0] = malloc(sizeof(struct _scanner_fa_transition) * SCANNER_FA_TRANSITION_INIT_SIZE);
    fa->_capacity_transitions = SCANNER_FA_TRANSITION_INIT_SIZE;
    fa->n_transitions = 0;
}

void scanner_fa_destroy(struct scanner_fa_128 *fa) {
    if (fa != NULL) {
        if (fa->transition != NULL) {
            if (fa->transition[0] != NULL) {
                free(fa->transition[0]);
                fa->transition[0] = NULL;
            }
            free(fa->transition);
        }
        free(fa);
    }
}

void scanner_fa_add_states(struct scanner_fa_128 *fa, unsigned char n_states_to_add) {
    unsigned char n_states_new = fa->n_states + n_states_to_add;
    realloc(fa->transition, n_states_new);

    for (unsigned char i = fa->n_states; i < n_states_new; i++) {
        fa->transition[i] = NULL;
    }

    fa->n_states = n_states_new;
}

/**
 * Right shifts the transition array of an FA from a specified index
 * 
 * @param from_element_i: the first index to be right shifted
 */
void _fa_trans_right_shift(struct scanner_fa_128 *fa, unsigned int from_element_i) {
    for (unsigned int i = fa->n_transitions; i >= from_element_i; i--) {
        fa->transition[0][i] = fa->transition[0][i-1];
    }
}

void _fa_state_right_shift(struct scanner_fa_128 *fa, unsigned int from_state) {
    for (unsigned int i = from_state; i < fa->n_states; i++) {
        if (fa->transition[i] != NULL) {
            fa->transition[i]++;
        }
    }
}

unsigned char _fa_find_closest_right(struct scanner_fa_128 *fa, unsigned char state) {
    unsigned char closest_right_i = state;
    while (closest_right_i < fa->n_states && fa->transition[closest_right_i] == NULL) ++closest_right_i;
    return closest_right_i;
}

void scanner_fa_add_transition(struct scanner_fa_128 * const fa, unsigned char state, unsigned char character, unsigned char next_state) {
    if (state == 0) {
        printf("WARNING: trying to add transition to the error-state. The attempt didn't have any consequences because you can't add transitions to the error state.");
        return;
    }

    if (next_state == 0) {
        printf("WARNING: adding transition from character to an error state is not necessary. Every undefined character transition will result in a transition to the error state;");
        return;
    }

    if (scanner_fa_next_state(fa, state, character) != 0) {
        printf("WARNING: adding already existing transition to state `%d` with char `%c`. This finite automaton will behave as an undeterministic FA from this point.", state, character);
    }
    
    // realloc if needed to give space for the new element
    fa->n_transitions++;
    if (fa->n_transitions >= fa->_capacity_transitions) {
        fa->_capacity_transitions *= 2;
        realloc(fa->transition[0], fa->_capacity_transitions);
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
        _fa_trans_right_shift(fa, fa->transition[closest_right_i] - fa->transition[0]);
        _fa_state_right_shift(fa, closest_right_i);

        inserted_elem = fa->transition[closest_right_i] - 1;
    }

    inserted_elem->c = character;
    inserted_elem->next_state = next_state;

    if (fa->transition[state] == NULL) {
        fa->transition[state] = inserted_elem;
    }
}

void scanner_fa_set_accepting(struct scanner_fa_128 * const fa, unsigned char state, unsigned char accepting) {
    if (state > fa->n_states) {
        printf("ERROR: scanner -> trying to set %d. state to accepting but FA only has %d of states.", state, fa->n_states);
        exit(EXIT_FAILURE);
    }
    int nth_int = (int)(state / fa->n_states); // determine which int to use
    int nth_bit = state - nth_int * 32;
    fa->accepting[nth_int] = fa->accepting[nth_int] ^ (accepting << nth_bit);
}

unsigned int scanner_fa_set_find_first_accepting(const unsigned int * const a) {
    unsigned int first_accepting = 0;
    for (unsigned int i = 0; i < 4; i++) {
        while ((a[i] >> first_accepting) & 1 == 0) {
            first_accepting++;

            if (first_accepting % 32 == 0) {
                break;
            }
        }
    }
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

unsigned char scanner_fa_is_accepting(const struct scanner_fa_128 * const fa, unsigned short state) {
    if (state > fa->n_states) {
        printf("ERROR: scanner -> trying to set %d. state to accepting but FA only has %d of states.", state, fa->n_states);
        exit(EXIT_FAILURE);
    }
    int nth_int = (int)(state / fa->n_states); // determine which int to use
    int nth_bit = state - nth_int * 32;
    return fa->accepting[nth_int] & ~(1 << nth_bit);
}

struct _scanner_fa_transition * _fa_find_closest_right_ptr(const struct scanner_fa_128 * const fa, unsigned char state) {
    unsigned char closest_right_i = _fa_find_closest_right(fa, state);

    struct _scanner_fa_transition *end;

    if (closest_right_i == fa->n_states) {
        end = fa->transition[0] + fa->n_transitions;
    } else {
        end = fa->transition[closest_right_i];
    }
    return end;
}

unsigned char scanner_dfa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch) {
    if (state == 0) {
        return 0;
    }

    struct _scanner_fa_transition *end = _fa_find_closest_right_ptr(fa, state);

    for (struct _scanner_fa_transition *i_ptr = fa->transition[state]; i_ptr != end; i_ptr++) {
        if (i_ptr->c == ch) return i_ptr->next_state;
    }
}

void scanner_nfa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch, struct cutils_arrayi ** next_states) {
    if (state == 0) {
        return 0;
    }

    if (*next_states == NULL) {
        cutils_arrayi_create();
    } else {
        cutils_arrayi_empty(*next_states);
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
        printf("ERROR: scanner_fa_merge -> resulting FA is too big. Aborting...");
        exit(EXIT_FAILURE);
    }

    int fa0_n = fa0->n_states;
    int fa0_n_transitions = fa0->n_transitions;

    scanner_fa_add_states(fa0, fa0->n_states + fa1->n_states - 1);

#if SCANNER_FA_TRANSITION_SCALING_FACTOR == 2
    fa0->_capacity_transitions = SCANNER_FA_TRANSITION_INIT_SIZE << (int)ceil(log2((fa0->n_transitions + fa1->n_transitions) / 10.0)); 
#else
    int scale_factor = SCANNER_FA_TRANSITION_SCALING_FACTOR;
    int init = SCANNER_FA_TRANSITION_INIT_SIZE;
    int t_sum = fa0->n_transitions + fa1->n_transitions;
    int scale = ceil(log10((t_sum)/10.0)/log10(scale_factor));
    fa0->_capacity_transitions = init * pow(scale_factor, scale);
#endif

    // copying char to next state array
    realloc(fa0->transition[0], fa0->_capacity_transitions);
    memcpy(fa0->transition[0]+fa0->n_transitions, fa1->transition[0], fa1->n_transitions);
    fa0->n_transitions += fa1->n_transitions;

    // copying state transition array
    struct _scanner_fa_transition *fa1_transitions_start = fa0->transition[0] + fa0_n_transitions;
    for (unsigned int i = 1; i < fa1->n_states; i++) {
        if (fa1->transition[i] != NULL) {
            fa0->transition[fa0_n - 1 + i] = fa1_transitions_start + (fa1->transition[i] - fa1->transition[0]);
        }
    }

    scanner_fa_set_union(fa0->accepting, fa1->accepting);
}

void scanner_fa_thompson_concat(struct scanner_fa_128 * const fa0, const struct scanner_fa_128 * const fa1) {
    int fa0_end_state = scanner_fa_set_find_first_accepting(fa0->accepting);
    int fa1_end_state = scanner_fa_set_find_first_accepting(fa1->accepting);
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
    scanner_fa_add_transition(fa0, fa0_end_state, 0x00, fa0_n_states);
}

void scanner_fa_thompson_alter(struct scanner_fa_128 * const fa0, const struct scanner_fa_128 * const fa1) {
    int fa0_initial = fa0->initial_state;
    int fa0_end = scanner_fa_set_find_first_accepting(fa0->accepting);

    int fa1_initial = fa1->initial_state + fa0->n_states - 1;
    int fa1_end = scanner_fa_set_find_first_accepting(fa1->accepting) + fa0->n_states - 1;

    scanner_fa_merge(fa0, fa1);

    // no accepting state remains accepting
    scanner_fa_set_zero(fa0->accepting);

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
    int fa_end = scanner_fa_set_find_first_accepting(fa->accepting);

    scanner_fa_set_zero(fa->accepting);

    scanner_fa_add_states(fa, 2);
    fa->initial_state = fa->n_states - 2;
    scanner_fa_set_accepting(fa, fa->n_states - 1, 1);

    scanner_fa_add_transition(fa, fa->initial_state, 0x00, fa_initial);
    scanner_fa_add_transition(fa, fa->initial_state, 0x00, fa->n_states - 1);
    scanner_fa_add_transition(fa, fa_end, 0x00, fa_initial);
    scanner_fa_add_transition(fa, fa_end, 0x00, fa->n_states - 1);
}