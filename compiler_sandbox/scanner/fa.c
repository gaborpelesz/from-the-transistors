// This file implements finite automatons with the ability to easily 
// concatenate them, thus building bigger "dfa"s

#include <stdio.h>
#include <stdlib.h>

typedef char bool;
#define false 0;
#define true 1;

typedef struct {
    short states_n; // this is the stride for the `map` as well
    short accepting_states_n;

    short *accepting_states;

    // implements the map function f(state, char) in a table
    // the error state is represented by -1
    // if this is an NFA, the empty string is encoded as 0x01
    short *map;
} FiniteAutomaton;

bool nfa_to_dfa(FiniteAutomaton *nfa);

bool dfa_process_text(FiniteAutomaton dfa, char *text) {
    short current_state = 0;
    for (unsigned int i = 0; text[i] != '\0'; i++) {
        current_state = dfa.map[current_state * 128 + text[i]];

        if (current_state == -1) {
            return false;
        }
    }

    for (unsigned int i = 0; i < dfa.accepting_states_n; i++) {
        if (current_state == dfa.accepting_states[i]) {
            return true;
        }
    }

    return false; // text not accepted
}

int main(void) {
    FiniteAutomaton dfa;
    
    // two state all ASCII alphabet
    dfa.states_n = 2;
    dfa.map = malloc(dfa.states_n * 128 * sizeof(*dfa.map));

    // ... try to create a dynamic finite automaton

    char n = -1;
    printf("%d\n", (int)n);

    free(dfa.map);
    free(dfa.accepting_states);

    return 0;
}