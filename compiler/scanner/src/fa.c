#include "../include/scanner_utils/fa.h"

#include <stdlib.h>

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
    // TODO ensure that the first pointer in transition,
    // always points to the first element in the second array
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
    fa->n_states += n_states_to_add;
    realloc(fa->transition, fa->n_states);
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

void scanner_fa_add_transition(struct scanner_fa_128 *fa, unsigned char state, unsigned char character, unsigned char next_state) {
    if (state == 0) {
        printf("WARNING: trying to add transition to the error-state. The attempt didn't have any consequences because you can't add transitions to the error state.");
        return;
    }

    if (next_state == 0) {
        printf("WARNING: adding transition from character to an error state is not necessary. Every undefined character transition will result in a transition to the error state;");
        return;
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

void scanner_fa_set_accepting(struct scanner_fa_128 *fa, unsigned char state, unsigned char accepting) {
    if (state > fa->n_states) {
        printf("ERROR: scanner -> trying to set %d. state to accepting but FA only has %d of states.", state, fa->n_states);
        exit(EXIT_FAILURE);
    }
    int nth_int = (int)(state / fa->n_states); // determine which int to use
    int nth_bit = state - nth_int * 32;
    fa->accepting[nth_int] = fa->accepting[nth_int] ^ (accepting << nth_bit);
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

unsigned char scanner_fa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch) {
    if (state == 0) {
        return 0;
    }

    unsigned char closest_right_i = _fa_find_closest_right(fa, state);

    struct _scanner_fa_transition *end;

    if (closest_right_i == fa->n_states) {
        end = fa->transition[0] + fa->n_transitions;
    } else {
        end = fa->transition[closest_right_i];
    }

    for (struct _scanner_fa_transition *i_ptr = fa->transition[state]; i_ptr != end; i_ptr++) {
        if (i_ptr->c == ch) return state;        
    }
}

struct scanner_fa_128 *scanner_fa_thompson_create_char(unsigned char character) {
}

void scanner_fa_thompson_alter(const struct scanner_fa_128 *fa0, const struct scanner_fa_128 * const fa1);
void scanner_fa_thompson_concat(const struct scanner_fa_128 *fa0, const struct scanner_fa_128 * const fa1);
void scanner_fa_thompson_close(const struct scanner_fa_128 *fa);