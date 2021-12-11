#include <scanner_utils/regex_scanner.h>

const char* scanner_regex_get_token_name(const unsigned int token) {
    if (token > 12) {
        return "";
    }

    const static char *regex_token_map[] = { "LITERAL",
                                        "PARENTHESIS_OPEN",
                                        "PARENTHESIS_CLOSE",
                                        "BRACKET_OPEN",
                                        "BRACKET_CLOSE",
                                        "ALTERATION",
                                        "CLOSURE",
                                        "POS_CLOSURE",
                                        "OP_ZERO_OR_ONE",
                                        "RANGE",
                                        "WILDCARD",
                                        "EMPTYSTR",
                                        "ANYWHITESPACE"
    };

    return regex_token_map[token];
}

static void append_to_string_array(struct cutils_string ***strarr, // pointer to an array of cutils_string
                            unsigned int *capacity,
                            unsigned int *size,
                            char *newstr) {
    // handle growing array's reallocation
    if (*size + 1 >= *capacity) {
        *capacity *= 2;
        (*strarr) = realloc((*strarr), sizeof(**strarr) * *capacity);
    }

    (*strarr)[*size] = cutils_string_create_from(newstr);

    *size += 1;
}

static void append_literal_ifany(struct cutils_string *literal,
                          struct cutils_arrayi *tokens,
                          struct cutils_string ***lexemes,
                          unsigned int *lexemes_capacity,
                          unsigned int *lexemes_size) {
    if (literal->size > 0) {
        cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_LITERAL);
        append_to_string_array(lexemes, lexemes_capacity, lexemes_size, literal->_s);
        cutils_string_empty(literal);   
    }
}

REGEX_SCANNER_STATUS scanner_regex_analyze(const struct cutils_string * const rgx,
                                   struct cutils_arrayi *tokens,
                                   struct cutils_string ***lexemes,
                                   unsigned int *lexemes_capacity,
                                   unsigned int *lexemes_n) {

    printf("scanning regex: %s\n", rgx->_s);

    int retval = 0;

    int parenthesis_cnt = 0;
    int b_is_bracket_open = 0;  
    int b_is_quote_literal = 0; 

    struct cutils_string *literal = cutils_string_create();

    for (unsigned int i = 0; i < rgx->size; i++) {
        char current_char = cutils_string_at(rgx, i);

        // dealing with possible special characters
        if (current_char == '\\') {
            // check if peak is possible, and if not, throw error
            if (i+1 == rgx->size) {
                retval = 1;
                break;
            }

            // peak is possible
            char peaked = cutils_string_at(rgx, i+1);

            // now check if we are dealing with quotes
            if (peaked == '"') {
                // escape quote detected -> considering quote as a literal
                cutils_string_append_chr(literal, '"');
                i = i+1; // we've already dealt with the next char
            }
            // backslash is always a special char even inside quoted literals
            // that means to use backslash as a literal, you have the option to escape it
            else if (peaked == '\\') {
                cutils_string_append_chr(literal, '\\'); 
                i = i+1; // we've already dealt with the next char
            }
            // only consider other special characters
            // if they aren't inside quotes (i.e., aren't taken as literals)
            else if (!b_is_quote_literal) {
                // handling a special character outside of a literal

                // any whitespace
                if (peaked == 's') {
                    append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                    cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_ANYWHITESPACE);
                    append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "\\s");
                }
                // tab is just a literal
                else if (peaked == 't') {
                    cutils_string_append_chr(literal, '\t');

                }
                // carriage return is just a literal
                else if (peaked == 'r') {
                    cutils_string_append_chr(literal, '\r');

                }
                // newline is just a literal
                else if (peaked == 'n') {
                    cutils_string_append_chr(literal, '\n');
                }
                // empty string
                else if (peaked == 'e') {
                    append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                    cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_EMPTYSTR);
                    append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "\\e");
                }

                i = i+1; // we've already dealt with the next char
            }
            // else taking backslash as a literal without considering peaked
            else {
                cutils_string_append_chr(literal, '\\');
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
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);


                // ensure that there are enough characters left for a range definition
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
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, range);

                i += 4;
            }
            // already handled by the opening bracket
            // so it is unexpected to stumble upon a closing bracket
            else if (current_char == ']') {
                printf("Unexpected closing bracket in %s at %d. char", rgx->_s, i);
                retval = 1;
                break;
            }

            else if (current_char == '(') {
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                parenthesis_cnt += 1;
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_PARENTHESIS_OPEN);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "(");
            }
            else if (current_char == ')') {
                // appending accumulated literal if there was any
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                parenthesis_cnt -= 1;
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_PARENTHESIS_CLOSE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, ")");
            }
            
            else if (current_char == '|') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_ALTERATION);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "|");
            }

            else if (current_char == '*') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_CLOSURE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "*");
            }

            else if (current_char == '+') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_POSCLOSURE);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "+");
            }

            else if (current_char == '?') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_OP_QUESTION_MARK);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, "?");
            }

            else if (current_char == '.') {
                append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);
                cutils_arrayi_push(tokens, SCANNER_REGEX_TOKEN_WILDCARD);
                append_to_string_array(lexemes, lexemes_capacity, lexemes_n, ".");
            }

            // no special character, append to the literal
            else {
                cutils_string_append_chr(literal, current_char);
            }
        }
        else { // current char is inside quotes -> handling it as literal
            cutils_string_append_chr(literal, current_char);
        }
    }

    // scanning...
    // ... add characters to a lexeme until we find a special character. The token for the lexeme: concatenated_chars
    // ... every string that's inside two quotes should be a concatenated_chars immediately
    // ... if the string inside two quotes contains a quote character it should be escaped with a backslash "\"this\"" matches -> "this"

    if (retval == 0) {
        // if there was no error, we might have some literals left to append
        append_literal_ifany(literal, tokens, lexemes, lexemes_capacity, lexemes_n);

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