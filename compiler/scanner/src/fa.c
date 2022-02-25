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

void scanner_fa_add_transition(struct scanner_fa_128 *fa, unsigned char state, unsigned char character, unsigned char next_state) {
    fa->n_transitions++;

    if (fa->n_transitions > fa->_capacity_transitions) {
        fa->_capacity_transitions *= 2;
        realloc(fa->transition[0], fa->_capacity_transitions);
    }
    
    // TODO: it turns out adding a transition isn't that simple. Imagine adding transitions to
    //       state_1 and then to state_3. When I want to get all the transitions at state_1 then I have
    //       to check where state_2 ends. state_2 is a nullptr, so I have to go further and check state_3 (?! yes this is possible).
    //       Also what if I want to finally add transitions to state_2? How do I manage the array for state_3?
    //
    // This is not impossible just requires rigorous planning and design, and correct and careful implementation


    if (fa->transition[state] == NULL) {
        // `state` didn't have any transitions previously.
        fa->transition[state] = fa->transition[0] + (fa->n_transitions - 1);
    } else {

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

struct scanner_fa_128 *scanner_fa_thompson_create_char(unsigned char character) {
    struct scanner_fa_128 *fa = malloc(sizeof(struct scanner_fa_128));
}

void scanner_fa_thompson_alter(const struct scanner_fa_128 *fa0, const struct scanner_fa_128 * const fa1);
void scanner_fa_thompson_concat(const struct scanner_fa_128 *fa0, const struct scanner_fa_128 * const fa1);
void scanner_fa_thompson_close(const struct scanner_fa_128 *fa);
unsigned char scanner_fa_next_state(const struct scanner_fa_128 * const fa, unsigned char state, char ch);