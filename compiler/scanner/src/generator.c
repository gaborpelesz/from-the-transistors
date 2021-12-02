#include <stdio.h>
#include <cutils/string.h>
#include <cutils/arrayi.h>

// What do we need to generate?
//  - FA states: #define FA_STATE_INITIAL 0; etc...
//    - list of accepting states: BOOL is_accepting_state[NUM_OF_STATES];
//  - transition table: short transition_table[NUM_OF_STATES][256];
//    - with the columns for the ASCII characters
//  - array to match token name indexes with states
//    - unsigned int classify_lexeme[NUM_OF_STATES] -> returns index for the token names array;
//  - tokens list with names
//    - this is the easiest I guess

#define SCANNER_REGEX_TOKEN_LITERAL            0 // ab
#define SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN   1 // (
#define SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE  2 // )
#define SCANNER_REGEX_TOKEN_BRACKETS_OPEN      3 // [
#define SCANNER_REGEX_TOKEN_BRACKETS_CLOSE     4 // ]
#define SCANNER_REGEX_TOKEN_OP_ALTERATION      5 // |
#define SCANNER_REGEX_TOKEN_OP_CLOSURE         6 // *
#define SCANNER_REGEX_TOKEN_OP_POSCLOSURE      7 // +
#define SCANNER_REGEX_TOKEN_OP_QUESTION_MARK   8 // ?
#define SCANNER_REGEX_TOKEN_RANGE           9 // [0-9]
#define SCANNER_REGEX_TOKEN_WILDCARD          10 // .
#define SCANNER_REGEX_TOKEN_EMPTYSTR          11 // \e
#define SCANNER_REGEX_TOKEN_TAB               12 // \t
#define SCANNER_REGEX_TOKEN_CARRIAGER         13 // \r
#define SCANNER_REGEX_TOKEN_NEWLINE           14 // \n
#define SCANNER_REGEX_TOKEN_ANYWHITESPACE     15 // \s

static const struct cutils_string* regex_token_map[6]; 

void append_to_string_array(struct cutils_string ***strarr,
                            unsigned int *size,
                            unsigned int *n,
                            struct cutils_string *newstr) {
    // handle growing array's reallocation
    if (*n + 1 >= *size) {
        *size *= 2;
        (*strarr) = realloc((*strarr), sizeof(**strarr) * *size);
    }

    (*strarr)[*n] = cutils_string_create_from(newstr->_s);

    *n += 1;
}

typedef int REGEX_SCANNER_STATUS;
REGEX_SCANNER_STATUS regex_scanner(const struct cutils_string * const rgx,
                                   struct cutils_arrayi *tokens,
                                   struct cutils_string ***lexemes,
                                   unsigned int *lexemes_size,
                                   unsigned int *lexemes_n) {

    printf("trying regex: %s\n", rgx->_s);

    int retval = 0;

    int parenthesis_cnt = 0;
    int b_is_bracket_open = 0;  
    int b_is_quote_literal = 0; 

    struct cutils_string *literal = cutils_string_create();

    for (unsigned int i = 0; i < rgx->size; i++) {
        char current_char = cutils_string_at(rgx, i);

        if (current_char == '"') {
            b_is_quote_literal = !b_is_quote_literal;

            //                         current_char __
            //                                        | 
            //                                        v
            // if a literal ended: "this was a literal"
            if (!b_is_quote_literal) {
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_LITERAL);

                // append to lexemes
                append_to_string_array(lexemes, lexemes_size, lexemes_n, literal);

                cutils_string_empty(literal);
            }
        }

        else if (!b_is_quote_literal) {
            if (current_char == '[') {
                // ensure that there is enough characters left for a range definition
                // a range is always 5 characters: [0-9] -> [<from>-<to>]
                if (i+4 >= rgx->size) {
                    printf("Unfinished range definition in %s -> '%s'\n", rgx->_s, rgx->_s + (rgx->size - i));
                    retval = 1;
                    break;
                }
                // checking if the range definition is correct
                else if (cutils_string_at(rgx, i+2) != '-' || cutils_string_at(rgx, i+4) != ']') {
                    printf("Incorrect range definition in %s -> '%s'\n", rgx->_s, rgx->_s + 4);
                    retval = 1;
                    break;
                }

                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_RANGE);

                char range[] = {'[', cutils_string_at(rgx, i+1), '-', cutils_string_at(rgx, i+3), ']', '\0'};
                append_to_string_array(lexemes, lexemes_size, lexemes_n, range);

                i += 4;
            }

            switch (current_char) {
                case '(':
                    printf("got a '('\n");
                    break;
            }
        } else { // current char is inside quotes -> handling it as literal

        }
    }
    // scanning...
    // ... add characters to a lexeme until we find a special character. The token for the lexeme: concatenated_chars
    // ... every string that's inside two quotes should be a concatenated_chars immediately
    // ... if the string inside two quotes contains a quote character it should be escaped with a backslash "\"this\"" matches -> "this"

    if (retval == 0) {
        if (parenthesis_cnt != 0) {
            if (parenthesis_cnt < 0) {
                printf("Unmatched parenthesis inside regex: too much closing ')'\n");
            } else { // (parenthesis_cnt < 0)
                printf("Unmatched parenthesis inside regex: too much opening '('\n");
            }
            retval = 1;
        }
        if (b_is_bracket_open != 0) {
            printf("Unmatched brackets inside regex: %s\n", rgx->_s);
            retval = 1;
        }
        if (b_is_quote_literal != 0) {
            printf("Unmatched quotes inside regex: %s\n", rgx->_s);
            retval = 1;
        }
    }

    cutils_string_destroy(literal);

    return retval;
}

// for testing
int main(void) {
    regex_token_map[0] = cutils_string_create_from("(");

    struct cutils_string *rgx = cutils_string_create_from("(ab|b)*");
    struct cutils_arrayi *tokens = cutils_arrayi_create();

    struct cutils_string **lexemes;
    lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int lexemes_size = 10;
    unsigned int lexemes_n = 0;

    regex_scanner(rgx, tokens, &lexemes, &lexemes_size, &lexemes_n);

    // deconstruct
    cutils_string_destroy(rgx);

    for (unsigned int i = 0; i < lexemes_n; i++) {
        cutils_string_destroy(lexemes[i]);
    }
    free(lexemes);
    // ~ deconstruct

    return 0;
}