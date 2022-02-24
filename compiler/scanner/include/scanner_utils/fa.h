#ifndef SCANNER_FINITE_AUTOMATON_H
#define SCANNER_FINITE_AUTOMATON_H

/** 
 * scanner_fa_128 implements a finite automaton with 
 * 127 possible states + error state with any ASCII transition
 * 
 * Finite automaton is a five-tuple (S, ∑, δ, s_0, S_A)
 *   - S: finite set of states (along with an error state)
 *   - ∑: finite alphabet (ASCII)
 *   - δ(s, c): transition function -> given a state and a character returns a next state
 *   - s_0: initial state
 *   - S_A: accepting states
 * 
 * Implementation:
 *   -     STATES: Inside the implementation the finite set is virtual, knowing the number of states is enough
 *                 and the fact that we go from state_0 to state_n.
 *                 the first state (state_0) is ALWAYS the error state.
 *   -   ALPHABET: We set the finite alphabet to be always equal to 128, representing the ASCII character set.
 *   -    INITIAL: We need to store the initial state
 *   -  ACCEPTING: bit-vector -> nth bit is accepting if == 1
 *   - TRANSITION: Most of the characters does not do anything (transition to error_state),
 *                 thus there is an obvious compression possibility here
 * 
 * Transition implementation notes
 * -------------------------------
 * simple sparse lookup-table implementation
 * 10 states would result in 1280 values to store 
 * 100 states would need 12800 values
 * 
 * if there is only one character which results in a state transition (for example language keywords)
 * then for the 100 states, there would only be 99 values that have any meaning other than state_error
 * 
 * Example: Keyword `unsigned` (could be also `continue`)
 *  - length: 8 characters (8 transitions)
 *  - states: 9 states + 1 error state
 * 
 * simple 2D lookup-table implementation would store 1152 integers
 * 
 */
struct scanner_fa_128 {
    unsigned char n_states;       // support for 127 state FA + error state (state_0)
    unsigned char initial_state;
    unsigned int accepting[4];    // bit-vector with 32*4 = 128 states
                                  // (n_states can handle 128 states at max)

    // This is not a 2D array. This is a pointer to an array of pointers.
    // The array of pointers point to specific locations in a second array.
    //
    // | *a0 | *a1 | *a2 | *a3 | *a4 | *a5 | *a6 | *a7 |
    //    |     |
    //    |     -------------      ...
    //    |                 |
    //    v                 v
    // | *b0 | *b1 | *b2 | *b3 | *b4 | *b5 | *b6 | *b7 |
    struct _scanner_fa_transition_ctos **transition;
};

/**
 * Helper struct for a more efficient implementation of table-lookup
 * represents a function of characters returning a next state
 */
struct _scanner_fa_transition_ctos {
    char c;
    unsigned char next_state;
};

unsigned char scanner_fa_is_accepting(struct scanner_fa_128 *fa, unsigned short state);

unsigned char scanner_fa_next_state(struct scanner_fa_128 *fa, unsigned char state, char ch);

#endif // SCANNER_FINITE_AUTOMATON_H