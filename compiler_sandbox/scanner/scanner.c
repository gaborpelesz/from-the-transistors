/**
 * developer: gaborpelesz@gmail.com
 * date: 21/10/2021
 * 
 * Development approach:
 *  Create a wireframe of the scanner.
 *  This is only how I would prefer to implement it
 *  and maybe not the most efficient or optimized way to do it.
 *  Later on we can make any changes and the scanner
 *  won't be that big of a component either.
 * 
 */

#include "array.h"
#include "stack.h"
#include "string.h"

// UTIL DEFINITIONS
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

// PRE-DEFINED GENERATED TABLES ---------

// sizes are only imaginary for now
BOOL accepting_states[10];        // indexed by state -> returns if the state is accepting (TRUE) or not (FALSE)
short transition_table[10][10];   // indexed by a state (1st) and a char (2nd) -> returns the corresponding next state
unsigned int classify_lexeme[10]; // indexed by accepting state -> returns index of token
struct string* tokens[10];        // list of token names

// --------------------------------------

/**
 * Given a text, the scanner algorithm separates the lexemes from eachother and classifies
 *  each of them into a syntactic category (token).
 * 
 * Inputs:
 *  -                              text: string of characters to analyze
 *  - (MODIFIES) analyzed_token_indices: the function creates a sequential list of the indices of tokens as it discovers them in the text
 *  - (MODIFIES)  analyzed_token_lexeme: the function creates a sequential list of lexemes as it discovers them in the text
 * 
 * Notes:
 *  The function appends the same amount of elements to both of the inputs that it modifies.
 *  The two arrays can be indexed with the same index to retreive the lexeme and its correspondent token.
 * 
 */
void scanner_skeleton_original(const struct string *const text,
                               struct array * const analyzed_token_indices,
                               struct array * const analyzed_token_lexemes) {
    // FA states:
    #define FA_STATE_INITIAL 0
    #define FA_STATE_ERROR -1
    #define FA_STATE_BAD -2 // this gets pushed to the stack at first

    int i = 0;

    struct string *lexeme = string_create();
    struct stack_short *state_stack = stack_short_create(); // instead of short, we could use arrays

    // Skeleton scanner FA table-driven simulation
    // original algorithm, can be optimized a lot
    while (i < text->size) {
        short current_state = 0;
        
        string_empty(lexeme);

        stack_short_empty(state_stack);
        stack_short_push(state_stack, -1); // push "bad"

        // forward pass until either we do not reach
        //  - a state from where we can't go further, or
        //  - end of text
        while (current_state != FA_STATE_ERROR || i < text->size) {
            string_add_char(lexeme, string_at(text, i));

            if (accepting_states[current_state]) {
                stack_short_empty(state_stack);
            }

            stack_short_push(state_stack, current_state);

            // very simple dummy usage on transition table, might be changed later...
            current_state = transition_table[current_state][string_at(text, i)];

            i = i+1; // next char
        }

        // rollback loop -> trying to find longest prefix that is accepting
        while (current_state != FA_STATE_BAD && !accepting_states[current_state]) {
            current_state = stack_short_pop(state_stack);
            string_pop(lexeme); // we don't need to save output
            i = i-1; // previous char
        }

        if (accepting_states[current_state]) {
            array_push(analyzed_token_indices, classify_lexeme[current_state]);
            array_push(analyzed_token_lexemes, lexeme);
        }
    }
}

void scanner_skeleton_custom() {
    /**
     * Similar but improved algorithm... WORK IN PROGRESS
     * Improvements:
     *  - precise line and column tracking of lexemes
     *  - better design for rollback (don't need to iterate backwards)
     * 
     * SLIGHTLY IMRPOVED - Python pseudo
     * ----------------------------------
     *  current_state = 0 #starting state
     *  current_lexeme = "" #init lexeme with empty string
     
     *  line_counter = 0 #number of lines
     *  line_counter_index = 0 #saves the current index of the '\n' character, thus
     *                         #when rollback happens, we doesn't count lines 2 times
     * 
     *  for i in range( length( text ) ):
     *      
     * 
     *      current_state = transition_table( text[i], current_state )
     *      current_lexeme = current_lexeme + text[i]  #string concat
     *  
     *      if accepting_states[current_state]:
     *          saved_accepting_state = current_state
     *          saved_accepting_lexeme = current_lexeme
     *          saved_last_char_index = i
     * 
     *      elif current_state == error_state: #ROLLBACK
     *          tokens.append([
     *                  classify_lexeme(saved_accepting_lexeme),
     *                  saved_line_counter,
     *                  saved_lexeme_column_start, #first char index in the column
     *                  saved_accepting_lexeme
     *              ])
     * 
     *          i = saved_last_char_index + 1 #rollback
     *          saved_lexeme_column_start = saved_last_char_index + 1
     *          saved_line_counter = line_counter
     * -----------------------------------
     * 
     * -
     * |
     * v
     * this is a character sequence
     *
     *
     */
}