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

// FOR DEBUG
void hello() {
    static int hello_counter = 0;
    printf("hello: %d\n", ++hello_counter);
}
// FOR DEBUG

// PRE-DEFINED GENERATED TABLES ---------

// FA states:
#define FA_STATE_INITIAL 0
#define FA_STATE_0 0
#define FA_STATE_1 1
#define FA_STATE_2 2
#define FA_STATE_ERROR 3
#define FA_STATE_BAD -1 // this gets pushed to the stack at first

// TODO sizes are only imaginary for now
static BOOL is_accepting_state[4];        // indexed by state -> returns if the state is accepting (TRUE) or not (FALSE)
static short transition_table[4][256];   // indexed by a state (1st) and a char (2nd) -> returns the corresponding next state
static unsigned int classify_lexeme[4]; // indexed by accepting state -> returns index of token
static struct cutils_string* tokens[2];        // list of token names

// --------------------------------------

/**
 * Given a text, the scanner algorithm separates the lexemes from eachother and classifies
 *  each of them into a syntactic category (token).
 * 
 * Inputs:
 *  -                              text: string of characters to analyze
 *  - (MODIFIES) token_classes: the function creates a sequential list of the indices of tokens as it discovers them in the text
 *  - (MODIFIES)  analyzed_token_lexeme: the function creates a sequential list of lexemes as it discovers them in the text
 * 
 * Notes:
 *  The function appends the same amount of elements to both 'token_classes' and 'token_lexemes'.
 *  (i.e., the two arrays can be indexed with the same index to retreive the lexeme and its correspondent token).
 */
void scanner_skeleton_original(const struct cutils_string *const text,
                               struct cutils_arrayi * const token_classes,
                               struct cutils_string ***token_lexemes) {
    
    int text_i = 0;

    *token_lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int token_lexemes_size = 10;
    unsigned int token_lexemes_n = 0;

    struct cutils_string *lexeme = cutils_string_create();
    struct cutils_arrayi *state_stack = cutils_arrayi_create();

    // Skeleton scanner FA table-driven simulation
    // original algorithm, can be optimized a lot
    while (text_i < text->size) {
        short current_state = FA_STATE_INITIAL;
        
        cutils_string_empty(lexeme);

        cutils_arrayi_empty(state_stack);
        cutils_arrayi_push(state_stack, FA_STATE_BAD); // push "bad"

        // forward pass until either we do not reach
        //  - a state from where we can't go further, or
        //  - end of text
        while (current_state != FA_STATE_ERROR && text_i < text->size) {
            cutils_string_append_chr(lexeme, cutils_string_at(text, text_i));

            if (is_accepting_state[current_state]) {
                cutils_arrayi_empty(state_stack);
            }

            cutils_arrayi_push(state_stack, current_state);

            // very simple dummy usage on transition table, might be changed later...
            current_state = transition_table[current_state][cutils_string_at(text, text_i)];
            
            text_i += 1; // next char
        }

        // rollback loop -> trying to find longest prefix that is accepting
        int n_rollback = 0;
        while (!is_accepting_state[current_state]) {
            current_state = cutils_arrayi_pop(state_stack);

            // breaking instead of pausing in the while, because
            // we don't want to pop one more lexeme when its already empty
            if (current_state == FA_STATE_BAD) {
                break;
            }

            n_rollback++; // counting rollback amount
            cutils_string_pop(lexeme); // we don't need to save output
        }

        unsigned int lexeme_class;

        // enough to check FA_STATE_BAD because either we stopped at an accepting state, or it was BAD
        if (current_state != FA_STATE_BAD) { // && true == is_accepting_state[current_state] 
            text_i -= n_rollback; // rolling back text to accepting state
            lexeme_class = classify_lexeme[current_state];
        } else {
            text_i -= n_rollback; // rolling back to beginning
            
            cutils_string_append_chr(lexeme, cutils_string_at(text, text_i));
            lexeme_class = classify_lexeme[FA_STATE_ERROR];

            text_i += 1; // going to next char as we have already been here
        }

        cutils_arrayi_push(token_classes, lexeme_class);

        // This is just to append a lexeme at the end of the 'cutils_string' list (very disgusting)
        // TODO create a dynamic array for this part...
        {
            // handle growing array's reallocation
            if (token_lexemes_n + 1 >= token_lexemes_size) {
                token_lexemes_size *= 2;
                (*token_lexemes) = realloc((*token_lexemes), sizeof(**token_lexemes) * token_lexemes_size);
            }
            
            (*token_lexemes)[token_lexemes_n] = cutils_string_create_from(lexeme->_s);

            token_lexemes_n += 1;
        }
        // end of TODO
    }

    cutils_string_destroy(lexeme);
    cutils_arrayi_destroy(state_stack);
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
     *      if is_accepting_state[current_state]:
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

// for initial testing
int main(void) {
    // Register name accepting FA settings; page 61 of 'Engineering a compiler'
    {
        is_accepting_state[FA_STATE_0] = FALSE; // error state
        is_accepting_state[FA_STATE_1] = FALSE;
        is_accepting_state[FA_STATE_2] = TRUE;
        is_accepting_state[FA_STATE_ERROR] = FALSE;

        // indexed by a state (1st) and a char (2nd) -> returns the corresponding next state
        // transition_table[4][256];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 256; j++) {
                transition_table[i][j] = FA_STATE_ERROR;
            }
        }

        transition_table[FA_STATE_0]['r'] = FA_STATE_1;

        transition_table[FA_STATE_1]['0'] = FA_STATE_2;
        transition_table[FA_STATE_1]['1'] = FA_STATE_2;
        transition_table[FA_STATE_1]['2'] = FA_STATE_2;
        transition_table[FA_STATE_1]['3'] = FA_STATE_2;
        transition_table[FA_STATE_1]['4'] = FA_STATE_2;
        transition_table[FA_STATE_1]['5'] = FA_STATE_2;
        transition_table[FA_STATE_1]['6'] = FA_STATE_2;
        transition_table[FA_STATE_1]['7'] = FA_STATE_2;
        transition_table[FA_STATE_1]['8'] = FA_STATE_2;
        transition_table[FA_STATE_1]['9'] = FA_STATE_2;

        transition_table[FA_STATE_2]['0'] = FA_STATE_2;
        transition_table[FA_STATE_2]['1'] = FA_STATE_2;
        transition_table[FA_STATE_2]['2'] = FA_STATE_2;
        transition_table[FA_STATE_2]['3'] = FA_STATE_2;
        transition_table[FA_STATE_2]['4'] = FA_STATE_2;
        transition_table[FA_STATE_2]['5'] = FA_STATE_2;
        transition_table[FA_STATE_2]['6'] = FA_STATE_2;
        transition_table[FA_STATE_2]['7'] = FA_STATE_2;
        transition_table[FA_STATE_2]['8'] = FA_STATE_2;
        transition_table[FA_STATE_2]['9'] = FA_STATE_2;

        classify_lexeme[FA_STATE_0] = 0;
        classify_lexeme[FA_STATE_1] = 0;
        classify_lexeme[FA_STATE_2] = 1;
        classify_lexeme[FA_STATE_ERROR] = 0;

        // TOKEN[0] will always hold the token category for ERROR
        // this is not defined by the user. This might not be considered as
        // an error, and the user might just skip on processing this.
        // strings with this TOKEN will always contain 1 character!
        tokens[0] = cutils_string_create_from("uncategorized");

        // the followings are user defined tokens!
        tokens[1] = cutils_string_create_from("register");
    }

    //struct cutils_string *text = cutils_string_create_from("rr");
    struct cutils_string *text = cutils_string_create_from("r3rr43r");
    //struct cutils_string *text = cutils_string_create_from("r");


    struct cutils_arrayi *token_classes = cutils_arrayi_create();
    struct cutils_string **token_lexemes;

    scanner_skeleton_original(text, token_classes, &token_lexemes);

    printf("result of tokenizing input: %s\n", text->_s);

    for (int i = 0; i < token_classes->size; i++) {
        int ti = cutils_arrayi_at(token_classes, i);
        printf("('%s' -> '%s')\n", token_lexemes[i]->_s, tokens[ti]->_s);
    }

    // FREEING UP EVERYTHING 
    cutils_string_destroy(text);

    for (int i = 0; i < token_classes->size; i++) {
        cutils_string_destroy(token_lexemes[i]);
    }

    cutils_arrayi_destroy(token_classes);

    cutils_string_destroy(tokens[0]);
    cutils_string_destroy(tokens[1]);

    return 0;
}