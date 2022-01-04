#include <scanner_utils/regex_parser.h>

#include <stdlib.h>

#include <cutils/arrayi.h>
#include <cutils/string.h>
#include <scanner_utils/regex_tree.h>

struct SCANNER_REGEX_STATUS scanner_regex_parse(const struct cutils_string * const rgx,
                                                struct scanner_regex_tree_node **root) {
    struct SCANNER_REGEX_STATUS ret;
    ret.type = SCANNER_REGEX_SUCCESS;
    ret.error_index = -1;
    ret.error_msg = "";

    int parenthesis_cnt = 0;
    int b_is_quote_literal = 0; 

    // Current alteration in the tree
    struct scanner_regex_tree_node *alt =
        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ALT, '|');

    // Current concatenation in the tree
    struct scanner_regex_tree_node *conc = 
        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.');

    scanner_regex_tree_add_child(alt, conc);

    // one-pass parsing through regex
    for (unsigned int i = 0; i < rgx->size; i++) {
        char current_char = cutils_string_at(rgx, i);

        // dealing with possible special characters
        if (current_char == '\\') {
            // check if peak is possible, and if not, return with error
            if (i+1 == rgx->size) {
                ret.type = SCANNER_REGEX_ERROR_BACKSLASH;
                ret.error_index = i;
                ret.error_msg = "Unexpected backslash at the end of the regex.";
                break;
            }

            // peak is possible
            char peaked = cutils_string_at(rgx, i+1);

            // now check if we are dealing with quotes
            if (peaked == '"') {
                // escape quote detected -> considering quote as a literal
                scanner_regex_tree_add_child(
                    conc,
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '"')
                );
                i = i+1; // we've already dealt with the next char
            }
            // backslash is always a special char even inside quoted literals
            // that means to use backslash as a literal, you have the option to escape it
            else if (peaked == '\\') {
                scanner_regex_tree_add_child(
                    conc,
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '\\')
                );
                i = i+1; // we've already dealt with the next char
            }
            // only consider other special characters
            // if they aren't inside quotes (i.e., aren't taken as literals)
            else if (!b_is_quote_literal) {
                // handling a special character outside of a literal

                // any whitespace
                if (peaked == 's') {
                    scanner_regex_tree_add_child(
                        conc,
                        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ANYWHITE, 's')
                    );
                }
                // tab is just a literal
                else if (peaked == 't') {
                    scanner_regex_tree_add_child(
                        conc,
                        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '\t')
                    );

                }
                // carriage return is just a literal
                else if (peaked == 'r') {
                    scanner_regex_tree_add_child(
                        conc,
                        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '\r')
                    );
                }
                // newline is just a literal
                else if (peaked == 'n') {
                    scanner_regex_tree_add_child(
                        conc,
                        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '\n')
                    );
                }
                // empty string
                else if (peaked == 'e') {
                    scanner_regex_tree_add_child(
                        conc,
                        scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_EMPTYLIT, 'e')
                    );
                }

                i = i+1; // we've already dealt with the next char
            }
            // else taking backslash as a literal without considering peaked
            else {
                scanner_regex_tree_add_child(
                    conc,
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, '\\')
                );
            }
        }
        else if (current_char == '"') {
            b_is_quote_literal = !b_is_quote_literal;

            //                         current_char __
            //                                        | 
            //                                        v
            // if a literal ended: "this was a literal"
            // 
            // it still doesn't need to be handled specially
            // it will be implicitly handled because we didn't include
            // the opening and closing quotes inside the literal
            // so we can just continue appending to it until we
            // stumble upon a special character
        }
        else if (!b_is_quote_literal) {
            // this also handles ']'
            if (current_char == '[') {
                // ensure that there are enough characters left for a range definition
                // a range is always 5 characters: [0-9] -> [<from>-<to>]
                if (i+4 >= rgx->size) {
                    ret.type = SCANNER_REGEX_ERROR_RANGE;
                    ret.error_index = i;
                    ret.error_msg = "Unfinished range definition...";
                    break;
                }
                // checking if the range definition is correct
                else if (cutils_string_at(rgx, i+2) != '-' || cutils_string_at(rgx, i+4) != ']' ||
                         !cutils_string_is_alphanum_c(cutils_string_at(rgx, i+1)) || !cutils_string_is_alphanum_c(cutils_string_at(rgx, i+3))) {
                    ret.type = SCANNER_REGEX_ERROR_RANGE;
                    ret.error_index = i;
                    ret.error_msg = "Incorrect range definition...";
                    break;
                }

                // range: left-child contains the left of the range,
                //        right-child contains the right of the range
                struct scanner_regex_tree_node *range = scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_RANGE, '[');
                scanner_regex_tree_add_child(conc, range);

                // appending left range 
                scanner_regex_tree_add_child(
                    range,
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_RANGE,
                                                   cutils_string_at(rgx, i+1))
                );

                // appending right range
                scanner_regex_tree_add_child(
                    range,
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_RANGE,
                                                   cutils_string_at(rgx, i+3))
                );

                i += 4;
            }
            // already handled by the opening bracket
            // so it is unexpected to stumble upon a closing bracket
            else if (current_char == ']') {
                //printf("Unexpected closing bracket in %s at %d. char", rgx->_s, i);
                ret.type = SCANNER_REGEX_ERROR_RANGE;
                ret.error_index = i;
                ret.error_msg = "Unexpected closing bracket...";
                break;
            }

            else if (current_char == '(') {
                parenthesis_cnt += 1;

                // appending a new alteration to the current concatenation
                struct scanner_regex_tree_node *new_alt =
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_ALT, '|');
                scanner_regex_tree_add_child(conc, new_alt);
                alt = new_alt;

                // appending a new concatenation to the new alteration and switching to it
                struct scanner_regex_tree_node *new_conc = 
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.');
                scanner_regex_tree_add_child(alt, new_conc);
                conc = new_conc;
            }
            else if (current_char == ')') {
                parenthesis_cnt -= 1;
                
                if (alt->parent == NULL) {
                    ret.type = SCANNER_REGEX_ERROR_CLOSINGP;
                    ret.error_index = i;
                    ret.error_msg = "Unexpected closing parenthesis...";
                    break;
                }

                alt = alt->parent->parent;
                conc = alt->children[alt->children_n-1];
            }
            
            else if (current_char == '|') {
                struct scanner_regex_tree_node *new_conc = 
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CONC, '.');
                scanner_regex_tree_add_child(alt, new_conc);     
                conc = new_conc;
            }

            else if (current_char == '*' ||
                     current_char == '+' ||
                     current_char == '?') {
                struct scanner_regex_tree_node * clos =
                    scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_CLOS, current_char);

                struct scanner_regex_tree_node *last = conc->children[conc->children_n-1];
                scanner_regex_tree_add_child(clos, last);
                conc->children[conc->children_n-1] = clos;
                clos->parent = conc;
            }

            else if (current_char == '.') {
                scanner_regex_tree_add_child(
                    conc, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_WILDCARD, '.')
                );
            }

            // no special character, append as a literal
            else {
                scanner_regex_tree_add_child(
                    conc, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, current_char)
                );
            }
        }
        else { // current char is inside quotes -> handling it as literal
            scanner_regex_tree_add_child(
                conc, scanner_regex_tree_create(SCANNER_REGEX_TREE_NODE_LITERAL, current_char)
            );
        }
    } // for char in regex

    if (ret.type == SCANNER_REGEX_SUCCESS && parenthesis_cnt > 0) {
            ret.type = SCANNER_REGEX_ERROR_OPENINGP;
            ret.error_index = rgx->size;
            ret.error_msg = "Unmatched parenthesis inside regex: too much opening '('\n";
    }

    // Find the root of the tree
    while (alt->parent != NULL) {
        alt = alt->parent;
    }
    *root = alt;

    // We destroy the tree on any error and set root to NULL;
    if (ret.type != SCANNER_REGEX_SUCCESS) {
        scanner_regex_tree_destroy(*root);
        *root = NULL;
        return ret;
    }

    // Minimize the tree so that single alterations and such aren't present
    scanner_regex_tree_minimize(root);

    return ret;
}