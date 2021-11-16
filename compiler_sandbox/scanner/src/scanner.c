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

#include <cutils/arrayi.h>
#include <cutils/string.h>

#include <stdio.h>
#include <stdlib.h>

// UTIL DEFINITIONS
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

// PRE-DEFINED GENERATED TABLES ---------

// TODO sizes are only imaginary for now
// TODO make these variables' scope local to this file
BOOL accepting_states[10];        // indexed by state -> returns if the state is accepting (TRUE) or not (FALSE)
short transition_table[10][10];   // indexed by a state (1st) and a char (2nd) -> returns the corresponding next state
unsigned int classify_lexeme[10]; // indexed by accepting state -> returns index of token
struct cutils_string* tokens[10];        // list of token names

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
 *  The function appends the same amount of elements to both 'analyzed_token_indices' and 'analyzed_token_lexeme'.
 *  (i.e., the two arrays can be indexed with the same index to retreive the lexeme and its correspondent token).
 */
void scanner_skeleton_original(const struct cutils_string *const text,
                               struct cutils_arrayi * const analyzed_token_indices,
                               struct cutils_string ** analyzed_token_lexemes) {
    // FA states:
    #define FA_STATE_INITIAL 0
    #define FA_STATE_ERROR -1
    #define FA_STATE_FIRST -2 // this gets pushed to the stack at first

    int text_i = 0;

    analyzed_token_lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int analyzed_token_lexemes_size = 10;
    unsigned int analyzed_token_lexemes_n = 0;

    struct cutils_string *lexeme = cutils_string_create();
    struct cutils_arrayi *state_stack = cutils_arrayi_create();
    
    // Skeleton scanner FA table-driven simulation
    // original algorithm, can be optimized a lot
    while (text_i < text->size) {
        short current_state = 0;
        
        cutils_string_empty(lexeme);

        cutils_arrayi_empty(state_stack);
        cutils_arrayi_push(state_stack, -1); // push "bad"

        // forward pass until either we do not reach
        //  - a state from where we can't go further, or
        //  - end of text
        while (current_state != FA_STATE_ERROR || text_i < text->size) {
            cutils_string_append_chr(lexeme, cutils_string_at(text, text_i));

            if (accepting_states[current_state]) {
                cutils_arrayi_empty(state_stack);
            }

            cutils_arrayi_push(state_stack, current_state);

            // very simple dummy usage on transition table, might be changed later...
            current_state = transition_table[current_state][cutils_string_at(text, text_i)];

            text_i += 1; // next char
        }

        // rollback loop -> trying to find longest prefix that is accepting
        while (current_state != FA_STATE_FIRST && !accepting_states[current_state]) {
            current_state = cutils_arrayi_pop(state_stack);
            cutils_string_pop(lexeme); // we don't need to save output
            text_i -= 1; // previous char
        }

        if (accepting_states[current_state]) {
            cutils_arrayi_push(analyzed_token_indices, classify_lexeme[current_state]);

            // This is just to append a lexeme at the end of the 'cutils_string' list (very disgusting)
            // TODO create a dynamic array for this part...
            {
                analyzed_token_lexemes[analyzed_token_lexemes_n] = cutils_string_create_from(lexeme->_s);
                analyzed_token_lexemes_n += 1;
                // handle growing array's reallocation
                if (analyzed_token_lexemes_n + 1 >= analyzed_token_lexemes_size) {
                    analyzed_token_lexemes_size *= 2;
                    analyzed_token_lexemes = realloc(analyzed_token_lexemes, sizeof(*analyzed_token_lexemes) * analyzed_token_lexemes_size);
                }
            }
            // end of TODO
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

int main(void) {
    printf("All works.\n");
    return 0;
}