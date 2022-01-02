#include <scanner_utils/regex_analyzer.h>

#ifdef UNIT_TESTING
    #include <cutils/cutils_unittest.h>
#endif

// What do we need to generate?
//  - FA states: #define FA_STATE_INITIAL 0; etc...
//    - list of accepting states: BOOL is_accepting_state[NUM_OF_STATES];
//  - transition table: short transition_table[NUM_OF_STATES][256];
//    - with the columns for the ASCII characters
//  - array to match token name indexes with states
//    - unsigned int classify_lexeme[NUM_OF_STATES] -> returns index for the token names array;
//  - tokens list with names
//    - this is the easiest I guess


// for testing
int main(void) {
    // REGEX TO BE TESTED
    //struct cutils_string *rgx = cutils_string_create_from("(ab|b)*");
    const char *test_rgx = "(a\"ab\\\"|cb\"|(asdf*b?))[0-9]\\s\\t\n end+";
    struct cutils_string *rgx = cutils_string_create_from(test_rgx);
    // struct cutils_string *rgx = cutils_string_create_from("(a(b\\\"|b+\\\"))?a*[0-9].v\\n");

    struct cutils_arrayi *tokens = cutils_arrayi_create();

    struct cutils_string **lexemes;
    lexemes = malloc(sizeof(struct cutils_string*) * 10);
    unsigned int lexemes_capacity = 10;
    unsigned int lexemes_n = 0;

    printf("scanning regex: %s\n", rgx->_s);
    int status = scanner_regex_analyze(rgx, tokens, &lexemes, &lexemes_capacity, &lexemes_n);
    printf("status: %d\n", status);

    printf("Tokens: %d, Lexemes: %d\n", tokens->size, lexemes_n);

    // print lexemes with tokens
    for (int i = 0; i < tokens->size; i++) {
        int ti = cutils_arrayi_at(tokens, i);
        printf("('%s' -> '%s')\n", lexemes[i]->_s, scanner_regex_get_token_name(ti));
    }

    // deconstruct
    cutils_string_destroy(rgx);

    for (unsigned int i = 0; i < lexemes_n; i++) {
        cutils_string_destroy(lexemes[i]);
    }
    free(lexemes);
    // ~ deconstruct

    return 0;
}