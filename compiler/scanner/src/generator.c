#include <stdio.h>

#include <scanner_utils/regex_parser.h>

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
    //const char *test_rgx = "(a|ab)*";
    const char *test_rgx = "(a\"ab\\\"|cb\"|(asdf*b?))[0-9]\\s\\t\n end+";
    struct cutils_string *rgx = cutils_string_create_from(test_rgx);

    struct scanner_regex_tree_node *root;

    printf("scanning regex: %s\n", rgx->_s);
    struct SCANNER_REGEX_STATUS status = scanner_regex_parse(rgx, &root);
    printf("status: %d\n", status.type);
    if (status.type != SCANNER_REGEX_SUCCESS) {
        printf("\t%s\n", status.error_msg);
    } else {
        scanner_regex_tree_print(root);
    }


    // deconstruct
    cutils_string_destroy(rgx);
    if (status.type == SCANNER_REGEX_SUCCESS) {
        scanner_regex_tree_destroy(root);
    }
    // ~ deconstruct

    return 0;
}