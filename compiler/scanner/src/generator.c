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

static const struct cutils_string* regex_token_map[6]; 

typedef int REGEX_SCANNER_STATUS;
REGEX_SCANNER_STATUS regex_scanner(const struct cutils_string * const rgx, struct cutils_arrayi *tokens) {
    printf("trying regex: %s\n", rgx->_s);

    int retval = 0;

    int parenthesis_cnt = 0;
    int bracket_cnt = 0;
    int quote_cnt = 0; // when update do: quote_cnt = !quote_cnt

    // scanning...
    // ... add characters to a lexeme until we find an special character. The token for the lexeme: concatenated_chars
    // ... every string that's inside two quotes should be a concatenated_chars immediately
    // ... if the string inside two quotes contains a quote character it should be escaped with a backslash "\"this\"" matches -> "this"

    if (parenthesis_cnt != 0) {
        if (parenthesis_cnt < 0) {
            printf("Unmatched parenthesis inside regex: too much closing ')'");
        } else { // (parenthesis_cnt < 0)
            printf("Unmatched parenthesis inside regex: too much opening '('");
        }
        retval = 1;
    }
    if (bracket_cnt != 0) {
        if (bracket_cnt < 0) {
            printf("Unmatched brackets inside regex: too much closing ')'");
        } else { // (parenthesis_cnt < 0)
            printf("Unmatched brackets inside regex: too much opening '('");
        }
        retval = 1;
    }
    if (quote_cnt != 0) {
        printf("Unmatched quotes inside regex: too much closing '\"'");
        retval = 1;
    }
    return retval;
}

// for testing
int main(void) {
    regex_token_map[0] = cutils_string_create_from("(");

    struct cutils_string *rgx = cutils_string_create_from("(ab|b)*");
    struct cutils_arrayi *tokens = cutils_arrayi_create();

    regex_scanner(rgx, tokens);

    cutils_string_destroy(rgx);
    return 0;
}