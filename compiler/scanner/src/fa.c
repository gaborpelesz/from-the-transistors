#include "../include/scanner_utils/fa.h"

#include <stdlib.h>

struct scanner_fa_128 *scanner_fa_create() {
    // TODO
    // - don't forget that the error state should already be added!
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
    // TODO if needed ...
}

void scanner_fa_add_transition(struct scanner_fa_128 *fa, unsigned char state, unsigned char character, unsigned char next_state) {
    // TODO if needed
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