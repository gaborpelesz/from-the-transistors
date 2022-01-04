#ifndef SCANNER_REGEX_PARSER_H_
#define SCANNER_REGEX_PARSER_H_

#include <scanner_utils/regex_tree.h>
#include <cutils/string.h>

enum SCANNER_REGEX_ERROR {
    SCANNER_REGEX_SUCCESS,
    SCANNER_REGEX_ERROR_BACKSLASH,
    SCANNER_REGEX_ERROR_RANGE,
    SCANNER_REGEX_ERROR_OPENINGP,
    SCANNER_REGEX_ERROR_CLOSINGP
};

struct SCANNER_REGEX_STATUS {
    enum SCANNER_REGEX_ERROR type;
    int error_index;
    char* error_msg;
};

/**
 * Parses a regex from a .lang file into an execution tree.
 * 
 * Implements a one-pass algorithm (complexity: O(n)) through the regex,
 * and creates a `regex_tree` of regex operations and literals. The tree
 * follows the precedence of the regex operations and can be used
 * to evaluate (execute) the regex.
 * The tree can be an input to generate the NFA of the regex.
 * 
 * @param rgx Input raw regex string
 * @param root Output root tree node of the precedence graph
 * @return Parsing status. The status will show if the parsing resulted in some errors.
 */
struct SCANNER_REGEX_STATUS scanner_regex_parse(const struct cutils_string * const rgx,
                                                struct scanner_regex_tree_node ** root);

#endif // SCANNER_REGEX_PARSER_H_